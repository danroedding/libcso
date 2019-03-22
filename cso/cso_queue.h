#ifndef __CSO_QUEUE_H
#define __CSO_QUEUE_H

typedef struct csoqueue_entry {
  struct csoqueue_entry	*next;
  container		*data;
} csoqueue_entry;

typedef struct {
  csoqueue_entry	*head;
  csoqueue_entry	**tail;
  char			*diag_id;
  int			queuelen;
} csoqueue;


/* csoq_new(): create a new queue
   returns ptr to queue object
*/
csoqueue *csoq_new(const char *diag_id);

/* csoq_delete(): delete a queue including all objects within
*/
void csoq_delete(csoqueue *queue);

/* csoq_put(): insert an object at end of queue
*/
void csoq_put(csoqueue *queue,container *req,int inherit);

/* csoq_get(): get an object from front of queue
   returns obj (inherited to caller) or NULL if queue empty
*/
container *csoq_get(csoqueue *queue);

/* csoq_count(): get number of objects in queue
   returns queue length (0 = queue is empty)
*/
int csoq_count(csoqueue *queue);


#endif


