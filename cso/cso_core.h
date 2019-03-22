#ifndef __CSO_CORE_H
#define __CSO_CORE_H

/* cso_new(): create an empty container object. diag_id is used for object
   statistics and TODO maybe for debugging purposes. May be supplied as NULL.
*/
container	*cso_new(const char *diag_id);

/* cso_delete(): delete a container object and all contents below */
void		cso_delete(container *obj);

/* cso_copyfrom(): copy data from one container into another
   does not delete existing data in destination object, i. e. merge of
   data possible
   returns 0 on success, nonzero on error(s)
*/
int		cso_copy(container *destobj,container *srcobj);

/* cso_delall(): delete all fields with in container. Container itself
   stays intact.
*/
void		cso_delall(container *obj);

/* cso_set_str(): set a string variable within container. Value is copied.
   returns 0 on success, 1 on field type error, negative on internal error
*/
int		cso_set_str(container *obj,const char *var,int arrindex,
			    const char *val);

/* cso_set_int(): set an int variable within container
   returns 0 on success, 1 on field type error, negative on internal error
*/
int		cso_set_int(container *obj,const char *var,int arrindex,
			    int val);

/* cso_set_obj(): set an object variable ("subcontainer") within container
   Caller may determine whether to copy or to directly incorporate the
   object to be added. If inherit flag is set, owner indicates that object
   is NOT to be copied, but directly to be incorporated.
   returns 0 on success, 1 on field type error, negative on internal error
*/
int		cso_set_obj(container *obj,const char *var,int arrindex,
			    container *val,int inherit);

/* cso_set_bin(): set a binary variable
   Caller may determine whether to copy or to directly incorporate the
   data buffer to be added. If inherit flag is set, owner indicates that
   buffer is NOT to be copied, but directly to be incorporated.
   returns 0 on success, 1 on field type error, negative on internal error

   Note on typization of data: due to the inherit feature, this must be
   declared as char * here. If you do not use the inherit feature, passed
   buffer will not be modified by cso_set_bin(), but an explicite ptr cast
   will probably be necessary in calling layer.
*/
int		cso_set_bin(container *obj,const char *var,
			    char *data,int datalen,int inherit);

/* cso_del(): delete a variable from container
   returns 0 on success, 1 if variable not found, negative on internal error
*/
int		cso_del(container *obj,const char *var);

/* cso_get_str(): get string value from container
   returns read-only ptr to value or NULL if not found/type error
*/
const char	*cso_get_str(container *obj,const char *var,int arrindex);

/* cso_get_int(): get integer value from container
   returns value or 0 if not found or bad type
   Warning: this means that in case of a return value of 0, caller may not
   know whether an error occurred or not
*/
int		cso_get_int(container *obj,const char *var,int arrindex);

/* cso_get_obj(): get object ("subcontainer") from container
   caller may decide to detach the requested subjobject from existing
   object structure. If detach flag is set, returned ptr is inherited to
   caller, which means caller must cso_delete() that after use. Detaching
   a subobject from a container also means that the reference to that obj
   is deleted from the field list (or array) of source container.
   returns value or NULL if not found
*/
container	*cso_get_obj(container *obj,const char *var,int arrindex,
			     int detach);

/* cso_get_bin(): get binary data variable from container
   returns 0 on success, 1 if variable not found, -1 on incompatible type
   additionally, data and datalen are filled after return
   caller may decide to detach the requested data buffer from existing
   object structure. If detach flag is set, returned ptr is inherited to
   caller, which means caller must free() that after use. Detaching
   a binary field from a container also means that the reference to that
   data is deleted from the field list of source container.
*/
int		cso_get_bin(container *obj,const char *var,int detach,
			    char **data,int *datalen);

/* cso_get_type(): return type of a container field
   returns CSO_* type info, where CSO_NONE means "field not found"
*/
cso_fieldtype	cso_get_type(container *obj,const char *var);

/* cso_get_arrsize(): get array size of a field
   returns array size, 0 if field not found or -1 on internal error
*/
int		cso_get_arrsize(container *obj,const char *var);

/* cso_force_array(): force a container array to be an array
   relevant for JSON serialization to distinguish between single values
   and arrays of element size 1
   returns 0 on success, 1 if field not found, -1 if operation not
   possible (e. g. because it is a binary field)
*/
int		cso_force_array(container *obj,const char *var);

#endif

