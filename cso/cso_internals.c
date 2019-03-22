/*
 * cso - cso_internals.c
 * =====================
 *
 * Developed in 2018 for OctoGate IT Security Systems GmbH.
 *
 * Contact at OctoGate: Frank Menne <frank.menne@hsm.de>
 *
 * Author(s):
 * - Daniel Rodding <daniel@roedding-software.de>
 * - Kevin Singh Arora <kevin@singharora.de>
 *
 * Software may be redistributed under the termins of the GNU General
 * Public License (Version 3 as of 29.06.2007).
 */

#include <stdlib.h>
#include <string.h>

#include "cso.h"

#ifdef USE_OBJSTAT
#include <objstat.h>
#endif


container *cso_find_path(container *obj,const char *var,const char **localvar,
			 int autocreate)
{
  const char *dot;
  char *newname;
  int cmp_varlen,cmp;
  cso_field *field;
  container *subobj;

  dot=strchr(var,'.');
  if (dot==NULL) {
    *localvar=var;
    return obj;
  }

  cmp_varlen=(int)((long)dot-(long)var);

  field=obj->fields;
  while (field!=NULL) {
    cmp=strncmp(field->name,var,cmp_varlen);
    if (!cmp) {
      if (field->type!=CSO_OBJECT)
	      return NULL;
      container *tmp=field->value.objval.arrdata[0];
      if(tmp==NULL)
        return NULL;
      return cso_find_path(tmp,dot+1,localvar,autocreate);
    }
    if (cmp>0)
      break;
    field=field->next;
  }

  if (!autocreate) return NULL;

  if (cmp_varlen<1) return NULL; /* inhibit subobjects with empty name */

  newname=malloc(cmp_varlen+1);
  if (newname==NULL) return NULL;

  memcpy(newname,var,cmp_varlen);
  newname[cmp_varlen]='\0';

  subobj=cso_new(obj->diag_id);
  if (subobj==NULL) {
    free(newname);
    return NULL;
  }

  cso_set_obj(obj,newname,0,subobj,1);
  free(newname);

  const char *new_dot=strchr(dot+1,'.');
  if (new_dot==NULL) {
    *localvar=dot+1;
    return subobj;
  } else {
    return cso_find_path(subobj,dot+1,localvar,autocreate);
  }
}

cso_field *cso_find_var(container *obj,const char *var)
{
  cso_field *p;

  p=obj->fields;
  while (p!=NULL) {
    if (!strcmp(p->name,var)) return p;
    p=p->next;
  }

  return NULL;
}

cso_field *cso_new_int_var(container *obj,const char *var)
{
  cso_field **pp,*p,*newobj;
  int cmp;

  if (var==NULL || !*var) return NULL; /* inhibit fields with empty name */

  pp=&obj->fields;
  while ((p= *pp)!=NULL) {
    cmp=strcmp(var,p->name);
    if (!cmp)
      return p->type==CSO_INT ? p : NULL;
    if (cmp<0) break;
    pp=&(p->next);
  }

  newobj=malloc(sizeof(cso_field));
  if (newobj==NULL) return NULL;

  newobj->name=strdup(var);
  if (newobj->name==NULL) {
    free(newobj); return NULL;
  }

  newobj->type=CSO_INT;
  newobj->arrayflag=0;

  newobj->value.intval.arrsize_logical=
  newobj->value.intval.arrsize_physical=1;
  newobj->value.intval.arrdata= &(newobj->value.intval.inline_val);
  newobj->value.intval.arrdata_allocated=0;
  newobj->value.intval.inline_val=0;
  newobj->next= *pp;
  *pp=newobj;

  return newobj;
}

cso_field *cso_new_str_var(container *obj,const char *var)
{
  cso_field **pp,*p,*newobj;
  int cmp;

  if (var==NULL || !*var) return NULL; /* inhibit fields with empty name */

  pp=&obj->fields;
  while ((p= *pp)!=NULL) {
    cmp=strcmp(var,p->name);
    if (!cmp)
      return p->type==CSO_STRING ? p : NULL;
    if (cmp<0) break;
    pp=&(p->next);
  }

  newobj=malloc(sizeof(cso_field));
  if (newobj==NULL) return NULL;

  newobj->name=strdup(var);
  if (newobj->name==NULL) {
    free(newobj); return NULL;
  }

  newobj->type=CSO_STRING;
  newobj->arrayflag=0;

  newobj->value.strval.arrsize_logical=
  newobj->value.strval.arrsize_physical=1;
  newobj->value.strval.arrdata=&(newobj->value.strval.inline_val);
  newobj->value.strval.arrdata_allocated=0;
  newobj->value.strval.inline_val=NULL;
  newobj->next= *pp;
  *pp=newobj;

  return newobj;
}

cso_field *cso_new_obj_var(container *obj,const char *var)
{
  cso_field **pp,*p,*newobj;
  int cmp;

  if (var==NULL || !*var) return NULL; /* inhibit fields with empty name */

  pp=&obj->fields;
  while ((p= *pp)!=NULL) {
    cmp=strcmp(var,p->name);
    if (!cmp)
      return p->type==CSO_OBJECT ? p : NULL;
    if (cmp<0) break;
    pp=&(p->next);
  }

  newobj=malloc(sizeof(cso_field));
  if (newobj==NULL) return NULL;

  newobj->name=strdup(var);
  if (newobj->name==NULL) {
    free(newobj); return NULL;
  }

  newobj->type=CSO_OBJECT;
  newobj->arrayflag=0;

  newobj->value.objval.arrsize_logical=
  newobj->value.objval.arrsize_physical=1;
  newobj->value.objval.arrdata=&(newobj->value.objval.inline_val);
  newobj->value.objval.arrdata_allocated=0;
  newobj->value.objval.inline_val=NULL;
  newobj->next= *pp;
  *pp=newobj;

  return newobj;
}

cso_field *cso_new_bin_var(container *obj,const char *var)
{
  cso_field **pp,*p,*newobj;
  int cmp;

  if (var==NULL || !*var) return NULL; /* inhibit fields with empty name */

  pp=&obj->fields;
  while ((p= *pp)!=NULL) {
    cmp=strcmp(var,p->name);
    if (!cmp)
      return p->type==CSO_BINARY ? p : NULL;
    if (cmp<0) break;
    pp=&(p->next);
  }

  newobj=malloc(sizeof(cso_field));
  if (newobj==NULL) return NULL;

  newobj->name=strdup(var);
  if (newobj->name==NULL) {
    free(newobj); return NULL;
  }

  newobj->type=CSO_BINARY;
  newobj->arrayflag=0;

  newobj->value.binval.data=NULL;
  newobj->value.binval.data_allocated=
  newobj->value.binval.data_len=0;
  newobj->next= *pp;
  *pp=newobj;

  return newobj;
}

int cso_expand_int_array(cso_field *field,int wantindex)
{
  int i,newsize,*newarray;

  if (wantindex<0 || field->type!=CSO_INT)
    return -1; /* paranoia */

  if (wantindex>=field->value.intval.arrsize_physical-1) {
    if (wantindex<10)
      newsize=wantindex+10;
    else
      newsize=wantindex*2;
    newarray=(int *)malloc(newsize*sizeof(int));
    if (newarray==NULL)
      return -1;

    i= -1;
    while (++i<field->value.intval.arrsize_logical)
      newarray[i]=field->value.intval.arrdata[i];
    while (i<newsize)
      newarray[i++]=0;

    if (field->value.intval.arrdata_allocated)
      free(field->value.intval.arrdata);
    else {
#ifdef USE_OBJSTAT
      objstat_add("cso_array_int");
#endif
    }

    field->value.intval.arrdata=newarray;
    field->value.intval.arrsize_physical=newsize;
    field->value.intval.arrdata_allocated=1;
  }

  if (wantindex>=field->value.intval.arrsize_logical)
    field->value.intval.arrsize_logical=wantindex;

  return 0;
}

int cso_expand_str_array(cso_field *field,int wantindex)
{
  int i,newsize;
  char **newarray;

  if (wantindex<0 || field->type!=CSO_STRING)
    return -1; /* paranoia */

  if (wantindex>=field->value.strval.arrsize_physical-1) {
    if (wantindex<10)
      newsize=wantindex+10;
    else
      newsize=wantindex*2;
    newarray=(char **)malloc(newsize*sizeof(char *));
    if (newarray==NULL)
      return -1;

    i= -1;
    while (++i<field->value.strval.arrsize_logical)
      newarray[i]=field->value.strval.arrdata[i];
    while (i<newsize)
      newarray[i++]=NULL;

    if (field->value.strval.arrdata_allocated)
      free(field->value.strval.arrdata);
    else {
#ifdef USE_OBJSTAT
      objstat_add("cso_array_string");
#endif
    }

    field->value.strval.arrdata=newarray;
    field->value.strval.arrsize_physical=newsize;
    field->value.strval.arrdata_allocated=1;
  }

  if (wantindex>=field->value.strval.arrsize_logical)
    field->value.strval.arrsize_logical=wantindex;

  return 0;
}

int cso_expand_obj_array(cso_field *field,int wantindex)
{
  int i,newsize;
  container **newarray;

  if (wantindex<0 || field->type!=CSO_OBJECT)
    return -1; /* paranoia */

  if (wantindex>=field->value.objval.arrsize_physical-1) {
    if (wantindex<10)
      newsize=wantindex+10;
    else
      newsize=wantindex*2;
    newarray=(container **)malloc(newsize*sizeof(container *));
    if (newarray==NULL)
      return -1;

    i= -1;
    while (++i<field->value.objval.arrsize_logical)
      newarray[i]=field->value.objval.arrdata[i];
    while (i<newsize)
      newarray[i++]=NULL;

    if (field->value.objval.arrdata_allocated)
      free(field->value.objval.arrdata);
    else {
#ifdef USE_OBJSTAT
      objstat_add("cso_array_obj");
#endif
    }

    field->value.objval.arrdata=newarray;
    field->value.objval.arrsize_physical=newsize;
    field->value.objval.arrdata_allocated=1;
  }

  if (wantindex>=field->value.objval.arrsize_logical)
    field->value.objval.arrsize_logical=wantindex;

  return 0;
}

static void cso_internal_del_field(cso_field *target)
{
  int i,arrsize;

  switch (target->type) {
    case CSO_STRING:
      i= -1;
      arrsize=target->value.strval.arrsize_logical;
      while (++i<arrsize)
	if (target->value.strval.arrdata[i]!=NULL)
	  free(target->value.strval.arrdata[i]);
      if (target->value.strval.arrdata_allocated) {
#ifdef USE_OBJSTAT
	objstat_del("cso_array_string");
#endif
	free(target->value.strval.arrdata);
      }
      break;

    case CSO_INT:
      if (target->value.intval.arrdata_allocated) {
#ifdef USE_OBJSTAT
	objstat_del("cso_array_int");
#endif
	free(target->value.intval.arrdata);
      }
      break;

    case CSO_OBJECT:
      i= -1;
      arrsize=target->value.objval.arrsize_logical;
      while (++i<arrsize)
	if (target->value.objval.arrdata[i]!=NULL)
	  cso_delete(target->value.objval.arrdata[i]);
      if (target->value.objval.arrdata_allocated) {
#ifdef USE_OBJSTAT
	objstat_del("cso_array_obj");
#endif
	free(target->value.objval.arrdata);
      }
      break;

    case CSO_BINARY:
      if (target->value.binval.data!=NULL)
	free(target->value.binval.data);
      break;

    default:
      /* TODO log illegal type */
      break;
  }

  free(target->name);
  free(target);
}

int cso_del_var(container *obj,const char *var)
{
  cso_field *p,**pp;

  pp=&(obj->fields);
  while ((p= *pp)!=NULL) {
    if (!strcmp(p->name,var)) {
      *pp=p->next;
      cso_internal_del_field(p);
      return 0;
    }
    pp=&(p->next);
  }

  return 1;
}

void cso_del_field(container *obj,cso_field *target)
{
  /* Notice: intented wrapper to allow for adding more code here */

  cso_internal_del_field(target);
}

