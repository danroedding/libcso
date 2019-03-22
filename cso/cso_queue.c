/*
 * cso - cso_queue.c
 * =================
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

#include <stdlib.h>
#include <string.h>

#include "cso.h"
#include <log.h>
#include <msgno.h>

#ifdef USE_OBJSTAT
#include <objstat.h>
#endif

#include "cso_queue.h"


csoqueue *csoq_new(const char *diag_id)
{
  csoqueue *q;

  q=(csoqueue *)malloc(sizeof(csoqueue));
  if (q==NULL) return NULL;

  q->diag_id=strdup(diag_id!=NULL ? diag_id : "csoqueue_generic");
  q->head=NULL;
  q->tail=&(q->head);
  q->queuelen=0;

#ifdef USE_OBJSTAT
  objstat_add(diag_id);
#endif

  return q;
}

void csoq_delete(csoqueue *queue)
{
  csoqueue_entry *p,*next;

  next=queue->head;
  while (next!=NULL) {
    p=next;
    next=p->next;
    cso_delete(p->data);
    free(p);
  }

#ifdef USE_OBJSTAT
  objstat_del(queue->diag_id);
#endif

  free(queue);
}

void csoq_put(csoqueue *queue,container *req,int inherit)
{
  container *req_copy;
  csoqueue_entry *qent;
  const char *cmd;

  if (req==NULL) {
    log_write(LOG_ERR,MSG_ERR_QUEUE_NULLOBJ,
      "csoqueue: attempt to enqueue NULL obj to queue '%s' (software bug)",
      queue->diag_id);
    return;
  }

  if (!inherit) {
    req_copy=cso_new("csoqueue_entry");
    cso_copy(req_copy,req);
    req=req_copy;
  }

  qent=(csoqueue_entry *)malloc(sizeof(csoqueue_entry));
  qent->data=req;
  qent->next=NULL;

  *(queue->tail)=qent;
  queue->tail=&(qent->next);
  queue->queuelen++;

  cmd=cso_get_str(req,"cmd",0);
  if (cmd==NULL) cmd="<none>";

  log_write(LOG_TRACE,0,
    "csoqueue: request queued, queue '%s', cmd='%s'",queue->diag_id,cmd);
}

container *csoq_get(csoqueue *queue)
{
  csoqueue_entry *p;
  container *req;
  const char *cmd;

  p=queue->head;
  if (p==NULL) return NULL; /* queue empty */

  queue->head=p->next;
  if (queue->head==NULL)
    queue->tail=&(queue->head);

  if (queue->queuelen>0)
    queue->queuelen--;
  else
    log_write(LOG_ERR,MSG_ERR_QUEUE_COUNTER,
      "csoqueue: queue len counter error on queue '%s' (software bug)",
      queue->diag_id);

  req=p->data;
  free(p);

  cmd=cso_get_str(req,"cmd",0);
  if (cmd==NULL) cmd="<none>";

  log_write(LOG_TRACE,0,
    "csoqueue: request dequeued, queue '%s', cmd='%s'",queue->diag_id,cmd);

  return req;
}

int csoq_count(csoqueue *queue)
{
  return queue->queuelen;
}


