/*
 * cso - cso_deserialize.c
 * =======================
 *
 * Developed in 2018 for OctoGate IT Security Systems GmbH.
 *
 * Contact at OctoGate: Frank Menne <frank.menne@hsm.de>
 *
 * Author(s):
 * - Kevin Singh Arora <kevin@singharora.de>
 *
 * Software may be redistributed under the termins of the GNU General
 * Public License (Version 3 as of 29.06.2007).
 */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "cso.h"

void cso_skipspace(const char **buf,int *buflen,int *size)
{
  while (*buflen>0 && *buf) {
    if (*buflen>1 && **buf=='/' && (*buf)[1]=='/') {
      // C++ style comment, skip to next line
      while (*buflen>0 && **buf) {
	if (**buf=='\n') break;
        (*buf)++; (*buflen)--; (*size)++;
      }
      if (*buflen<1) break;
    }

    if (**buf!=' ' && **buf!='\t' && **buf!='\r' && **buf!='\n')
      break;
    (*buf)++; (*buflen)--; (*size)++;
  }
}

int cso_parseint(const char **buf,int *buflen,int *size)
{
  int first=1,negative=0,val=0;

  while (*buflen>0) {
    if (**buf=='-' && first)
      negative=1;
    else if (!isdigit(**buf))
      break;
    else {
      if (first) val=**buf-'0';
      else {
	val*=10;
	val+=**buf-'0';
      }
    }
    first=0;
    (*buf)++; (*buflen)--; (*size)++;
  }

  return negative ? -val : val;
}

const char *cso_parsestring(const char **buf,int *buflen,int *size,char **deserBuf,int *deserBufLen)
{
  char *outp,c,*newbuf;
  int escape=0,xflag=0,xval,outcount;

  if (*deserBuf==NULL) {
    *deserBuf=(char*)malloc(1000);
    if (*deserBuf==NULL) {
      return NULL;
    }
    *deserBufLen=1000;
  }

  outp=*deserBuf;
  outcount=0;

  while (*buflen>0 && (**buf!='"' || escape)) {
    if (**buf=='\\') {
      if (escape) {
	*(outp++)='\\'; outcount++;
	escape=0;
      }
      else {
        escape=1;
      }
      xflag=0;
    }
    else if (escape) {
      if (**buf=='n') {
	*(outp++)='\n'; outcount++;
      }
      else if (**buf=='r') {
	*(outp++)='\r'; outcount++;
      }
      else if (**buf=='"') {
	*(outp++)='"'; outcount++;
      }
      else if (**buf=='x')
	xflag=1;
      // TODO: parse octal values, do we need that?
      escape=0;
    }
    else if (xflag==1) {
      c=toupper(**buf);
      if (isdigit(c))
	xval=(c-'0')*16;
      else if (c>='A' && c<='F')
	xval=(c-'A'+10)*16;
      else
	xval=0;
      xflag=2;
    }
    else if (xflag==2) {
      c=toupper(**buf);
      if (isdigit(c))
	xval+=c-'0';
      else if (c>='A' && c<='F')
	xval+=c-'A'+10;
      xflag=0;
      *(outp++)=(char)xval;
      outcount++;
    }
    else {
      *(outp++)= **buf;
      outcount++;
    }
    (*buf)++; (*buflen)--; (*size)++;

    if (outcount>=*deserBufLen-2) {
      newbuf=(char *)malloc(*deserBufLen*2);
      if (newbuf==NULL) {
	      return NULL;
      }

      memcpy(newbuf,*deserBuf,outcount);
      free(*deserBuf);
      *deserBuf=newbuf;
      *deserBufLen=*deserBufLen*2;
      outp=*deserBuf+outcount;
    }
  }

  if (*buflen>0 && **buf=='"') {
    (*buf)++; (*buflen)--; (*size)++;
  }
  *outp='\0';

  return *deserBuf;
}

int cso_parsearray(container *destobj,const char *varName,const char **buf,
			   int *buflen,int *size,char **deserBuf,int *deserBufLen)
{
  cso_fieldtype dataType=CSO_NONE;
  int arrIndex=0,valInt,subSize;
  const char *valString;
  container *subObj;

  if (*buflen<1 || **buf!='[')
    return -1;

  (*buf)++; (*buflen)--; (*size)++;

  for (;;) {
    cso_skipspace(buf,buflen,size);
    if (*buflen<1)
      return -1;

    if (**buf==']')
      break;

    if (**buf=='"') {
      if (dataType==CSO_NONE) dataType=CSO_STRING;
      if (dataType!=CSO_STRING)
	      return -1;

      (*buf)++; (*buflen)--; (*size)++;
      valString=cso_parsestring(buf,buflen,size,deserBuf,deserBufLen);
      if (valString==NULL)
	      return -1;

      if (*buflen>0 && **buf=='"') {
        (*buf)++; (*buflen)--; (*size)++;
      }

      cso_set_str(destobj,varName,arrIndex++,valString);

    }
    else if (**buf=='{') {
      if (dataType==CSO_NONE) dataType=CSO_OBJECT;
      if (dataType!=CSO_OBJECT) {
	      return -1;
      }
      subObj=cso_new("obj");
      if (subObj==NULL) {
	      return -1;
      }
      subSize=cso_deserialize(subObj,*buf,*buflen);
      if (subSize<0) {
        cso_delete(subObj);
        return -1;
      }
      (*buf)+=subSize;
      (*buflen)-=subSize;
      (*size)+=subSize;
      cso_set_obj(destobj,varName,arrIndex++,subObj,1);
    }
    else if (isdigit(**buf) || **buf=='-') {
      if (dataType==CSO_NONE) dataType=CSO_INT;
      if (dataType!=CSO_INT) {
	      return -1;
      }
      valInt=cso_parseint(buf,buflen,size);
      cso_set_int(destobj,varName,arrIndex++,valInt);
    }
    else {
      return -1;
    }

    cso_skipspace(buf,buflen,size);

    if (*buflen<1) {
      return -1;
    }

    if (**buf==']')
      break;

    if (**buf!=',') {
      return -1;
    }

    (*buf)++; (*buflen)--; (*size)++;
  }

  (*buf)++; (*buflen)--; (*size)++;

  cso_force_array(destobj,varName); // yeah, it's an array! Forever!

  return *size;
}

int cso_deserialize(container *destobj,const char *buf,int buflen)
{
  int size=0,subSize;
  char varName[100];
  const char *valString;
  int varNameLen,valInt;
  char *deserBuf=NULL;
  int deserBufLen=0;

  if(buf==NULL || buflen<1)
    return -1;

  cso_skipspace(&buf,&buflen,&size);
  if (buflen<1 || *buf!='{') {
    return -1;
  }

  buf++; buflen--; size++;

  for (;;) {
    cso_skipspace(&buf,&buflen,&size);

    if (buflen>0 && *buf=='}')
      break;

    if (buflen<1 || *buf!='"')
      goto bailout;

    buf++; buflen--; size++;

    varNameLen=0;
    while (buflen>0 && *buf!='"') {
      varName[varNameLen++]= *(buf++);
      size++; buflen--;
      if (varNameLen>98) {
	      varName[99]='\0';
        goto bailout;
      }
    }
    varName[varNameLen]='\0';
    if (buflen>0) {
      buf++; buflen--; size++;
    }

    cso_skipspace(&buf,&buflen,&size);

    if (buflen<1 || *buf!=':')
      goto bailout;

    buf++; buflen--; size++;

    cso_skipspace(&buf,&buflen,&size);

    if (buflen<1)
      goto bailout;

    if (*buf=='[') {
      subSize=cso_parsearray(destobj,varName,&buf,&buflen,&size,&deserBuf,&deserBufLen);
      if (subSize<0)
	      goto bailout;
    }
    else if (*buf=='{') {
      container *subObj=cso_new("obj");
      if (subObj==NULL)
	      goto bailout;
      subSize=cso_deserialize(subObj,buf,buflen);
      if (subSize<0) {
        cso_delete(subObj);
        goto bailout;
      }
      buf+=subSize;
      buflen-=subSize;
      size+=subSize;
      cso_set_obj(destobj,varName,0,subObj,1);
    }
    else if (*buf=='"') {
      buf++; buflen--; size++;
      valString=cso_parsestring(&buf,&buflen,&size,&deserBuf,&deserBufLen);
      if (valString==NULL) {
	      goto bailout;
      }
      if (buflen>0 && *buf=='"') {
	      buf++; buflen--; size++;
      }
      cso_set_str(destobj,varName,0,valString);
    }
    else if (isdigit(*buf) || *buf=='-') {
      valInt=cso_parseint(&buf,&buflen,&size);
      cso_set_int(destobj,varName,0,valInt);
    }
    else if (*buf=='@' && buf[1]=='@') {
      buf+=2; buflen-=2; size+=2;
      valInt=cso_parseint(&buf,&buflen,&size);
      if (buflen<0 || *buf!='@' || valInt<0) {
        goto bailout;
      }
      buf++; buflen--; size++;
      if (buflen<valInt+2) {
	      goto bailout;
      }
      cso_set_bin(destobj,varName,(char *)buf,valInt,0);
      buf+=valInt; buflen-=valInt; size+=valInt;
      if (buflen<2 || *buf!='@' || buf[1]!='@') {
	      goto bailout;
      }
      buf+=2; buflen-=2; size+=2;
    }
    else {
      goto bailout;
    }

    cso_skipspace(&buf,&buflen,&size);

    if (buflen<1) {
      goto bailout;
    }

    if (*buf=='}')
      break;

    if (*buf!=',') {
      goto bailout;
    }

    buf++; buflen--; size++;
  }

  size++;

  if (deserBuf!=NULL) {
    free(deserBuf);
    deserBuf=NULL;
    deserBufLen=0;
  }

  return size;

bailout:
  if (deserBuf!=NULL) {
    free(deserBuf);
    deserBuf=NULL;
    deserBufLen=0;
  }

  return -1;
}

