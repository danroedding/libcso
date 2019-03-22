#ifndef __CSO_SERIALIZE_HPP
#define __CSO_SERIALIZE_HPP


/* cso_serialize(): serialize a container to JSON
   returns buffer, length and error status (0 = success, nonzero else)
   Caution: in case of errors, a partial buffer may be returned
*/
int cso_serialize(container *obj,char **buf,int *buflen);

/* cso_serialize_to_buffer(): serialize a container to preallocated buffer
   caller provides buffer, there is NO(!) out-of-bounds check currently!!!
*/
int cso_serialize_to_buffer(container *obj,char *buf,int buflen);

/* cso_get_serialized_size(): inquire required buffer size for serialization
*/
int cso_get_serialized_size(container *obj);

#endif

