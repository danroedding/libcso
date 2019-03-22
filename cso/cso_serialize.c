/*
 * cso - cso_serialize.c
 * =====================
 *
 * Developed in 2018 for OctoGate IT Security Systems GmbH.
 *
 * Contact at OctoGate: Frank Menne <frank.menne@hsm.de>
 *
 * Author(s):
 * - Daniel Rodding <daniel@roedding-software.de>
 *
 * Software may be redistributed under the termins of the GNU General
 * Public License (Version 3 as of 29.06.2007).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cso.h"


#define CSO_MAX_STRLEN_INT 13 /* enough for 32bit integers */


int cso_serialize(container *obj,char **buf,int *buflen)
{
  char *outbuf;
  int outbuflen,rc;

  outbuflen=cso_get_serialized_size(obj)+1;
  outbuf=(char *)malloc(outbuflen);
  if (outbuf==NULL)
    rc= -1;
  else
    rc=cso_serialize_to_buffer(obj,outbuf,outbuflen);

  *buf=outbuf;
  *buflen=rc>0 ? rc : outbuflen;
  return rc>0 ? 0 : -1;
}

static int cso_ser_int(cso_field *field,char *buf,int buflen)
{
  int i=-1,l,lsum=0;

  if (field->value.intval.arrsize_logical<2 && !field->arrayflag) {
    if (buflen<CSO_MAX_STRLEN_INT+1)
      return -1;
    sprintf(buf,"%d",field->value.intval.arrdata[0]);
    return strlen(buf);
  }

  if (buflen<5)
    return -1;

  strcpy(buf,"[ ");
  buf+=2; buflen-=2; lsum+=2;

  while (++i<field->value.intval.arrsize_logical) {
    if (buflen<3+CSO_MAX_STRLEN_INT)
      return -1;

    sprintf(buf,"%d%s",
	    field->value.intval.arrdata[i],
	    i==field->value.intval.arrsize_logical-1 ? " ] " : ", ");

    l=strlen(buf);
    buf+=l; buflen-=l; lsum+=l;
  }

  return lsum;
}

static int cso_ser_str(cso_field *field,char *buf,int buflen)
{
  int i=-1;
  int l,lsum=0;
  const char *ip;
  char *op;

  if (buflen<2)
    return -1;

  if (field->value.strval.arrsize_logical>1 || field->arrayflag) {
    strcpy(buf,"[ ");
    buf+=2; buflen-=2; lsum+=2;
  }

  if (buflen<10)
    return -1;

  while (++i<field->value.strval.arrsize_logical) {
    if (buflen<4)
      return -1;

    ip=field->value.strval.arrdata[i];
    l=ip!=NULL ? strlen(ip) : 0;

    *buf='"';
    op=buf+1;
    l=0;

    while (ip!=NULL && *ip) {
      if (l+7>buflen)
        return -1;

      if (*ip=='\\') {
        *(op++)='\\'; *(op++)='\\'; l+=2;
      } else if (*ip=='\n') {
        *(op++)='\\'; *(op++)='n'; l+=2;
      } else if (*ip=='\"') {
        *(op++)='\\'; *(op++)='"'; l+=2;
      } else if (*ip>=0 && *ip<32) {
        *(op++)='\\';
        *(op++)='x';
        *(op++)="0123456789ABCDEF"[(*ip/16)&0xf];
        *(op++)="0123456789ABCDEF"[(*ip)%16];
        l+=4;
      } else {
        *(op++)= *ip; l++;
      }
      ip++;
    }
    *(op++)='"';

    if (i!=field->value.strval.arrsize_logical-1)
      { *(op++)=','; *(op++)=' '; }
    *op='\0';

    l=strlen(buf);
    buflen-=l; buf+=l; lsum+=l;
  }

  if (field->value.strval.arrsize_logical>1 || field->arrayflag) {
    strcpy(buf," ]");
    buf+=2; buflen-=2; lsum+=2;
  }

  return lsum;
}

static int cso_ser_obj(cso_field *field,char *buf,int buflen)
{
  /* WARNING: This function does NOT check the buflen! */

  int i=-1,size=0,rc;

  if (field->value.objval.arrsize_logical>1 || field->arrayflag) {
    strcpy(buf,"[ ");
    buf+=2; size+=2;
  }

  while (++i<field->value.objval.arrsize_logical) {
    if (field->value.objval.arrdata[i]==NULL) {
      strcpy(buf,"{}");
      buf+=2; size+=2;
    }
    else {
      rc=cso_serialize_to_buffer(field->value.objval.arrdata[i],buf,buflen);
      if (rc<0) return -1;
      buf+=rc;
      size+=rc;
    }

    if (i!=field->value.objval.arrsize_logical-1) {
      strcpy(buf,",\n");
      buf+=2; size+=2;
    }
  }

  if (field->value.objval.arrsize_logical>1 || field->arrayflag) {
    strcpy(buf," ]");
    buf+=2; size+=2;
  }

  return size;
}

static int cso_ser_bin(cso_field *field,char *buf,int buflen)
{
  int l=0;

  if (buflen<field->value.binval.data_len+5+CSO_MAX_STRLEN_INT)
    return -1;

  sprintf(buf,"@@%d@",field->value.binval.data_len);
  l=strlen(buf);
  buf+=l;

  if (field->value.binval.data!=NULL && field->value.binval.data_len>0) {
    memcpy(buf,field->value.binval.data,field->value.binval.data_len);
    buf+=field->value.binval.data_len;
    l+=field->value.binval.data_len;
  }

  strcpy(buf,"@@");
  l+=2;

  return l;
}

int cso_serialize_to_buffer(container *obj,char *buf,int buflen)
{
  cso_field *p;
  int first=1,size=0,l;

  if (buflen<5)
    return -1;

  strcpy(buf,"{ ");
  buf+=2; buflen-=2; size+=2;

  p=obj->fields;
  while (p!=NULL) {
    l=strlen(p->name);
    if (buflen<5+l) return -1;

    if (first)
      first=0;
    else {
      strcpy(buf,",\n");
      buf+=2; buflen-=2; size+=2;
    }

    *(buf++)='"';
    buflen--; size++;

    strcpy(buf,p->name);
    buf+=l; buflen-=l; size+=l;

    strcpy(buf,"\":");
    buf+=2; buflen-=2; size+=2;

    switch (p->type) {
      case CSO_STRING:
	l=cso_ser_str(p,buf,buflen);
	break;
      case CSO_INT:
	l=cso_ser_int(p,buf,buflen);
	break;
      case CSO_OBJECT:
	l=cso_ser_obj(p,buf,buflen);
	break;
      case CSO_BINARY:
	l=cso_ser_bin(p,buf,buflen);
	break;
      default:
	return -1; /* illegal data in container */
    }

    if (l<0)
      return -1;

    buf+=l; buflen-=l; size+=l;

    p=p->next;
  }

  if (buflen<2)
    return -1;

  strcpy(buf,"\n}");
  size+=2;
  return size;
}

static int cso_sersize_str(cso_field *field)
{
  int i=-1,size=0;
  const char *p;

  while (++i<field->value.strval.arrsize_logical) {
    size+=4;
    p=field->value.strval.arrdata[i];
    if (p!=NULL) {
      while (*p) {
	if (*p<32) size+=4; /* worst case: \xNN = 4 bytes */
	else if (*p=='\\' || *p=='"') size+=2;
	else size++;
	p++;
      }
    }
  }

  if (field->arrayflag || field->value.strval.arrsize_logical>1)
    size+=4;

  return size;
}

static int cso_sersize_int(cso_field *field)
{
  /* TODO this heuristics only works for integers no wider than 32bit */

  return CSO_MAX_STRLEN_INT*field->value.intval.arrsize_logical+1;
}

static int cso_sersize_obj(cso_field *field)
{
  int i=-1,size=0,rc;
  container *p;

  while (++i<field->value.objval.arrsize_logical) {
    size+=6;
    p=field->value.objval.arrdata[i];
    if (p!=NULL) {
      rc=cso_get_serialized_size(p);
      if (rc<0) return -1;
      size+=rc;
    }
  }

  if (field->arrayflag || field->value.strval.arrsize_logical>1)
    size+=4;

  return size;
}

static int cso_sersize_bin(cso_field *field)
{
  return field->value.binval.data_len+5+CSO_MAX_STRLEN_INT;
}

int cso_get_serialized_size(container *obj)
{
  cso_field *p;
  int sercount=0,rc;

  p=obj->fields;
  while (p!=NULL) {
    sercount+=strlen(p->name)+6;

    switch(p->type) {
      case CSO_STRING:
	rc=cso_sersize_str(p);
	break;

      case CSO_INT:
	rc=cso_sersize_int(p);
	break;

      case CSO_OBJECT:
	rc=cso_sersize_obj(p);
	break;

      case CSO_BINARY:
	rc=cso_sersize_bin(p);
	break;

      default:
	rc=0;
	break;
    }

    if (rc<0) return -1;

    sercount+=rc;
    p=p->next;
  }

  sercount+=5; /* safety margin */

  return sercount;
}

