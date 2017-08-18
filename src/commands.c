#include <stdlib.h>
#include <assert.h>
#include "commands.h"
#include "controller.h"

history_stack_t *init_history_stack(void) {
  history_stack_t *hs = malloc(sizeof(history_stack_t));
  hs->top = NULL;
  hs->bottom = NULL;

  return hs;
}

future_queue_t *init_future_queue(void) {
  future_queue_t *fq = malloc(sizeof(future_queue_t));
  fq->front = NULL;
  fq->back = NULL;

  return fq;
}

void destroy_history_stack(history_stack_t *hs) {
  command_t *c = hs->bottom, *tmp;

  while (c) {
    tmp = c;
    c = c->next;
    free(tmp);
  }
  free(hs);
}

void destroy_future_queue(future_queue_t *fq) {
  command_t *c = fq->front, *tmp;

  while (c) {
    tmp = c;
    c = c->next;
    free(tmp);
  }
  free(fq);
}

command_t *init_command(COMMAND_TYPE t, COMMAND_PAYLOAD p) {
  command_t *c = malloc(sizeof(command_t));
  c->type = t;
  c->payload = p;
  c->next = NULL;
  c->prev = NULL;
  return c;
}

// -------------------------
// -- History Stack methods
// -------------------------
command_t *append_command(history_stack_t *hs, command_t *c) {
  // assert valid state
  assert((!hs->top && !hs->bottom) || (hs->top && hs->bottom));

  // when hs is empty
  if (!hs->top && !hs->bottom) {
    hs->top = c;
    hs->bottom = c;
    return c;
  }

  c->prev = hs->top;
  hs->top->next = c;
  hs->top = c;
  return c;
}

command_t *pop_command(history_stack_t *hs) {
  assert((!hs->top && !hs->bottom) || (hs->top && hs->bottom));

  if (!hs->top && !hs->bottom) {
    return NULL;
  }

  command_t *popped = hs->top;

  // if there is only one command in stack
  if (popped->prev == NULL) {
    hs->top = NULL;
    hs->bottom = NULL;
    return popped;
  }

  hs->top = popped->prev;
  popped->prev->next = NULL;
  popped->prev = NULL;
  return popped;
}


// -------------------------
// -- Future Queue methods
// -------------------------
command_t *queue_command(future_queue_t *fq, command_t *c) {
  assert((!fq->front && !fq->back) || (fq->front && fq->back));

  if (!c) {
    return NULL;
  }

  if (!fq->front && !fq->back) {
    fq->front = c;
    fq->back = c;
    return c;
  }

  c->prev = fq->back;
  fq->back->next = c;
  fq->back = c;

  return c;
}
