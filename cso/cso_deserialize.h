#ifndef __CSO_DESERIALIZE_H
#define __CSO_DESERIALIZE_H

/* cso_deserializze(): deserialize from buffer with JSON input
   returns number of bytes deserialized, -1 on error
*/
int cso_deserialize(container *destobj,const char *buf,int buflen);

#endif

