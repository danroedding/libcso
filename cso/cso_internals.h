#ifndef __CSO_INTERNALS_H
#define __CSO_INTERNALS_H

/* internal service functions for cso implementation */

container	*cso_find_path(container *obj,const char *var,
			       const char **localvar,int autocreate);

cso_field	*cso_find_var(container *obj,const char *var);
cso_field	*cso_new_int_var(container *obj,const char *var);
cso_field	*cso_new_str_var(container *obj,const char *var);
cso_field	*cso_new_obj_var(container *obj,const char *var);
cso_field	*cso_new_bin_var(container *obj,const char *var);

int		cso_expand_int_array(cso_field *field,int wantindex);
int		cso_expand_str_array(cso_field *field,int wantindex);
int		cso_expand_obj_array(cso_field *field,int wantindex);

int		cso_del_var(container *obj,const char *var);
void		cso_del_field(container *obj,cso_field *target);

#endif

