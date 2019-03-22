/*
 * cso - cso-core.c
 * ================
 *
 * Developed in 2018, 2019 for OctoGate IT Security Systems GmbH.
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

#include <strtools.h>

#include "cso.h"
#include "cso_internals.h"

#ifdef USE_OBJSTAT
#include <objstat.h>
#endif


container *cso_new(const char *diag_id)
{
  container *newobj;

  newobj=malloc(sizeof(container));
  if (newobj==NULL) return NULL;

  newobj->fields=NULL;
  newobj->diag_id=strdup((diag_id!=NULL && *diag_id) ? diag_id : "cso_default");

#ifdef USE_OBJSTAT
  objstat_add(newobj->diag_id);
  objstat_add("cso_all");
#endif

  return newobj;
}

void cso_delete(container *obj)
{
  cso_delall(obj);

#ifdef USE_OBJSTAT
  objstat_del(obj->diag_id);
  objstat_del("cso_all");
#endif

  if (obj->diag_id!=NULL) free(obj->diag_id);
  free(obj);
}

int cso_copy(container *destobj,container *srcobj)
{
  cso_field *cur_field;
  const char *strval;
  char *binval;
  int arrsize,i,intval,errflag=0,binval_len;
  container *objval;

  cur_field=srcobj->fields;
  while (cur_field!=NULL) {
    arrsize=cso_get_arrsize(srcobj,cur_field->name);

    switch (cur_field->type) {
      case CSO_STRING:
	i= -1;
	while (++i<arrsize) {
	  strval=cso_get_str(srcobj,cur_field->name,i);
	  errflag|=cso_set_str(destobj,cur_field->name,i,strval);
	}
	break;

      case CSO_INT:
	i= -1;
	while (++i<arrsize) {
	  intval=cso_get_int(srcobj,cur_field->name,i);
	  errflag|=cso_set_int(destobj,cur_field->name,i,intval);
	}
	break;

      case CSO_OBJECT:
	i= -1;
	while (++i<arrsize) {
	  objval=cso_get_obj(srcobj,cur_field->name,i,0);
	  errflag|=cso_set_obj(destobj,cur_field->name,i,objval,0);
	}
	break;

      case CSO_BINARY:
	if (!cso_get_bin(srcobj,cur_field->name,0,&binval,&binval_len))
	  errflag|=cso_set_bin(destobj,cur_field->name,binval,binval_len,0);
	else
	  errflag=1;
        break;

      default:
	errflag=1;
	break;
    }

    cur_field=cur_field->next;
  }

  return errflag ? -1 : 0;
}

void cso_delall(container *obj)
{
  cso_field *cur_field;

  while (obj->fields!=NULL) {
    cur_field=obj->fields;
    obj->fields=obj->fields->next;
    cso_del_field(obj,cur_field);
  }
}

int cso_set_str(container *obj,const char *var,int arrindex,const char *val)
{
  container *destobj;
  const char *localvar;
  cso_field *fielddata;

  destobj=cso_find_path(obj,var,&localvar,1);
  if (destobj==NULL || localvar==NULL) return -1;

  fielddata=cso_find_var(destobj,localvar);
  if (fielddata==NULL) {
    fielddata=cso_new_str_var(destobj,localvar);
    if (fielddata==NULL) return -1;
  }

  switch (fielddata->type) {
    case CSO_STRING:
      if (arrindex &&
	  cso_expand_str_array(fielddata,arrindex+1))
	return -1;
      if (arrindex>0) fielddata->arrayflag=1;
      if (fielddata->value.strval.arrdata[arrindex]!=NULL)
	free(fielddata->value.strval.arrdata[arrindex]);
      fielddata->value.strval.arrdata[arrindex]=val!=NULL ? strdup(val) : NULL;
      return 0;

    case CSO_INT:
      if (arrindex &&
	  cso_expand_int_array(fielddata,arrindex+1))
	return -1;
      if (arrindex>0) fielddata->arrayflag=1;
      if (!str_contains_intval(val))
	return 1; /* cannot convert */
      fielddata->value.intval.arrdata[arrindex]=atoi(val);
      return 0;

    case CSO_OBJECT:
    case CSO_BINARY:
      return 1; /* cannot convert object or binary to string */

    default:
      return -1; /* unknown field type?! */
  }
}

int cso_set_int(container *obj,const char *var,int arrindex,int val)
{
  container *destobj;
  const char *localvar;
  cso_field *fielddata;
  static char cso_set_int_setbuf[50]; /* for string-to-int autoconversions */

  destobj=cso_find_path(obj,var,&localvar,1);
  if (destobj==NULL || localvar==NULL) return -1;

  fielddata=cso_find_var(destobj,localvar);
  if (fielddata==NULL) {
    fielddata=cso_new_int_var(destobj,localvar);
    if (fielddata==NULL) return -1;
  }

  switch (fielddata->type) {
    case CSO_STRING:
      if (arrindex &&
	  cso_expand_str_array(fielddata,arrindex+1))
	return -1;
      if (arrindex>0) fielddata->arrayflag=1;
      if (fielddata->value.strval.arrdata[arrindex]!=NULL)
	free(fielddata->value.strval.arrdata[arrindex]);
      sprintf(cso_set_int_setbuf,"%d",val);
      fielddata->value.strval.arrdata[arrindex]=strdup(cso_set_int_setbuf);
      return 0;

    case CSO_INT:
      if (arrindex &&
	  cso_expand_int_array(fielddata,arrindex+1))
	return -1;
      if (arrindex>0) fielddata->arrayflag=1;
      fielddata->value.intval.arrdata[arrindex]=val;
      return 0;

    case CSO_OBJECT:
    case CSO_BINARY:
      return 1; /* cannot convert object or binary to integer */

    default:
      return -1; /* unknown field type?! */
  }
}

int cso_set_obj(container *obj,const char *var,int arrindex,container *val,
		int inherit)
{
  container *destobj,*newobj;
  const char *localvar;
  cso_field *fielddata;

  destobj=cso_find_path(obj,var,&localvar,1);
  if (destobj==NULL || localvar==NULL) return -1;

  fielddata=cso_find_var(destobj,localvar);
  if (fielddata==NULL) {
    fielddata=cso_new_obj_var(destobj,localvar);
    if (fielddata==NULL) return -1;
  }

  if (fielddata->type!=CSO_OBJECT)
    return 1;

  if (arrindex && cso_expand_obj_array(fielddata,arrindex+1))
    return -1;
  if (arrindex>0) fielddata->arrayflag=1;
  if (fielddata->value.objval.arrdata[arrindex]!=NULL)
    cso_delete(fielddata->value.objval.arrdata[arrindex]);
  if (inherit)
    fielddata->value.objval.arrdata[arrindex]=val;
  else {
    newobj=fielddata->value.objval.arrdata[arrindex]=cso_new(destobj->diag_id);
    if (newobj==NULL)
      return -1;
    if (val!=NULL)
      cso_copy(newobj,val);
  }
  return 0;
}

int cso_set_bin(container *obj,const char *var,
		char *data,int datalen,int inherit)
{
  container *destobj;
  const char *localvar;
  cso_field *fielddata;

  destobj=cso_find_path(obj,var,&localvar,1);
  if (destobj==NULL || localvar==NULL) return -1;

  fielddata=cso_find_var(destobj,localvar);
  if (fielddata==NULL) {
    fielddata=cso_new_bin_var(destobj,localvar);
    if (fielddata==NULL) return -1;
  }

  if (fielddata->type!=CSO_BINARY)
    return 1;

  if (fielddata->value.binval.data!=NULL)
    free(fielddata->value.binval.data);
  if (inherit) {
    fielddata->value.binval.data=data;
    fielddata->value.binval.data_allocated=
    fielddata->value.binval.data_len=datalen;
  }
  else if (data!=NULL) {
    fielddata->value.binval.data=(char *)malloc(datalen);
    if (fielddata->value.binval.data==NULL) {
      fielddata->value.binval.data_allocated=
      fielddata->value.binval.data_len=0;
      return -1;
    }
    memcpy(fielddata->value.binval.data,data,datalen);
    fielddata->value.binval.data_allocated=
    fielddata->value.binval.data_len=datalen;
  }
  else {
    fielddata->value.binval.data=NULL;
    fielddata->value.binval.data_allocated=
    fielddata->value.binval.data_len=0;
  }
  return 0;
}

int cso_del(container *obj,const char *var)
{
  container *destobj;
  const char *localvar;

  destobj=cso_find_path(obj,var,&localvar,0);
  if (destobj==NULL || localvar==NULL) return 1;

  return cso_del_var(destobj,localvar);
}

const char *cso_get_str(container *obj,const char *var,int arrindex)
{
  container *destobj;
  const char *localvar;
  cso_field *fielddata;
  static char cso_get_str_getbuf[50]; /* for int-to-string autoconversions */

  destobj=cso_find_path(obj,var,&localvar,0);
  if (destobj==NULL || localvar==NULL) return NULL;

  fielddata=cso_find_var(destobj,localvar);
  if (fielddata==NULL) return NULL;

  switch (fielddata->type) {
    case CSO_STRING:
      if (arrindex<0 || arrindex>=fielddata->value.strval.arrsize_logical)
	return NULL;
      return fielddata->value.strval.arrdata[arrindex];

    case CSO_INT:
      if (arrindex<0 || arrindex>=fielddata->value.intval.arrsize_logical)
	return NULL;
      sprintf(cso_get_str_getbuf,"%d",
	      fielddata->value.intval.arrdata[arrindex]);
      return cso_get_str_getbuf;

    case CSO_OBJECT:
    case CSO_BINARY:
      return NULL; /* cannot convert */

    default:
      return NULL; /* unknown element */
  }
}

int cso_get_int(container *obj,const char *var,int arrindex)
{
  container *destobj;
  const char *localvar;
  cso_field *fielddata;

  destobj=cso_find_path(obj,var,&localvar,0);
  if (destobj==NULL || localvar==NULL) return 0;

  fielddata=cso_find_var(destobj,localvar);
  if (fielddata==NULL) return 0;

  switch (fielddata->type) {
    case CSO_STRING:
      if (arrindex<0 || arrindex>=fielddata->value.strval.arrsize_logical)
	return 0;
      return atoi(fielddata->value.strval.arrdata[arrindex]);

    case CSO_INT:
      if (arrindex<0 || arrindex>=fielddata->value.intval.arrsize_logical)
	return 0;
      return fielddata->value.intval.arrdata[arrindex];

    case CSO_OBJECT:
    case CSO_BINARY:
      return 0; /* cannot convert */

    default:
      return 0; /* unknown element */
  }
}

container *cso_get_obj(container *obj,const char *var,int arrindex,
		       int detach)
{
  container *destobj,*retobj;
  const char *localvar;
  cso_field *fielddata;

  destobj=cso_find_path(obj,var,&localvar,0);
  if (destobj==NULL || localvar==NULL) return NULL;

  fielddata=cso_find_var(destobj,localvar);
  if (fielddata==NULL) return NULL;

  if (fielddata->type!=CSO_OBJECT)
    return NULL; /* cannot convert from/to object */

  if (arrindex<0 || arrindex>=fielddata->value.objval.arrsize_logical)
    return NULL;

  retobj=fielddata->value.objval.arrdata[arrindex];
  if (detach)
    fielddata->value.objval.arrdata[arrindex]=NULL;
  return retobj;
}

int cso_get_bin(container *obj,const char *var,int detach,
		char **data,int *datalen)
{
  container *destobj;
  const char *localvar;
  cso_field *fielddata;

  destobj=cso_find_path(obj,var,&localvar,0);
  if (destobj==NULL || localvar==NULL) return 1;

  fielddata=cso_find_var(destobj,localvar);
  if (fielddata==NULL) return 1;

  if (fielddata->type==CSO_STRING) {
    *data=fielddata->value.strval.arrdata[0];
    if (*data!=NULL)
      *datalen=strlen(*data);
    else
      *datalen=0;

    if (detach)
      fielddata->value.strval.arrdata[0]=NULL;

    return 0;
  }

  if (fielddata->type!=CSO_BINARY)
    return -1; /* cannot convert from/to object */

  *data=fielddata->value.binval.data;
  *datalen=fielddata->value.binval.data_len;

  if (detach) {
    fielddata->value.binval.data=NULL;
    fielddata->value.binval.data_len=
    fielddata->value.binval.data_allocated=0;
  }

  return 0;
}

cso_fieldtype cso_get_type(container *obj,const char *var)
{
  container *destobj;
  const char *localvar;
  cso_field *fielddata;

  destobj=cso_find_path(obj,var,&localvar,0);
  if (destobj==NULL || localvar==NULL) return CSO_NONE;

  fielddata=cso_find_var(destobj,localvar);
  if (fielddata==NULL) return CSO_NONE;

  return fielddata->type;
}

int cso_get_arrsize(container *obj,const char *var)
{
  container *destobj;
  const char *localvar;
  cso_field *fielddata;

  destobj=cso_find_path(obj,var,&localvar,0);
  if (destobj==NULL || localvar==NULL) return 0;

  fielddata=cso_find_var(destobj,localvar);
  if (fielddata==NULL) return 0;

  switch (fielddata->type) {
    case CSO_STRING:
      return fielddata->value.strval.arrsize_logical;
    case CSO_INT:
      return fielddata->value.intval.arrsize_logical;
    case CSO_OBJECT:
      return fielddata->value.objval.arrsize_logical;
    case CSO_BINARY:
      return 1; /* binary fields cannot be arrays */
    default:
      return -1;
  }
}

int cso_force_array(container *obj,const char *var)
{
  container *destobj;
  const char *localvar;
  cso_field *fielddata;

  destobj=cso_find_path(obj,var,&localvar,0);
  if (destobj==NULL || localvar==NULL) return 1;

  fielddata=cso_find_var(destobj,localvar);
  if (fielddata==NULL) return 1;

  if (fielddata->type==CSO_BINARY)
    return -1; /* binary fields are never arrays by design */

  fielddata->arrayflag=1;
  return 0;
}


