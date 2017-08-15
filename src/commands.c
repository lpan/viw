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

static void dispatch_command(state_t *st, command_t *c) {
  switch (c->type) {
    case HANDLE_MOVE:
      handle_move(st, c->payload.d);
      break;
    case HANDLE_MOVE_TO_EDGE:
      handle_move_to_edge(st, c->payload.d);
      break;
    case HANDLE_INSERT_CHAR:
      handle_insert_char(st, c->payload.c);
      break;
    case HANDLE_APPEND_ROW:
      handle_append_row(st);
      break;
    case HANDLE_PREPEND_ROW:
      handle_prepend_row(st);
      break;
    case HANDLE_DELETE_CHAR:
      handle_delete_char(st);
      break;
    case HANDLE_DELETE_ROW:
      handle_delete_row(st);
      break;
    case HANDLE_ENTER:
      handle_enter(st);
      break;
    case HANDLE_BACKSPACE:
      handle_backspace(st);
      break;
  }
}

static command_t *init_command(COMMAND_TYPE t, COMMAND_PAYLOAD p) {
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
static void append_command(history_stack_t *hs, command_t *c) {
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

static command_t *pop_command(history_stack_t *hs) {
  assert((!hs->top && !hs->bottom) || (hs->top && hs->bottom));

  if (!hs->top && !hs->bottom) {
    return NULL;
  }

  command_t *popped = hs->top;

  // if there is only one command in stack
  if (popped->prev == NULL) {
    hs->top = NULL;
    hs->bottom = NULL;
  }

  popped->prev->next = NULL;
  popped->prev = NULL;
  return popped;
}


// -------------------------
// -- Future Queue methods
// -------------------------
static void queue_command(future_queue_t *fq, command_t *c) {
  assert((!fq->front && !fq->back) || (fq->front && fq->back));

  if (!fq->front && !fq->back) {
    fq->front = c;
    fq->back = c;
    return;
  }

  c->prev = fq->back;
  fq->back->next = c;
  fq->back = c;
}


// -------------------------
// -- Public methods
// -------------------------

void replay_history(state_t *st) {
  history_stack_t *hs = st->hs;
  command_t *c = hs->bottom;

  while (c) {
    dispatch_command(st, c);
    c = c->next;
  }
}

/*
 * 1. create command
 * 2. push it to stack
 * 3. execute the command against state
 */
void apply_command(state_t *st, COMMAND_TYPE t, COMMAND_PAYLOAD p) {
  command_t *c = init_command(t, p);
  append_command(st->hs, c);
  dispatch_command(st, c);
}

/*
 * 1. pop history stack
 * 2. push the result from 1. to future queue
 * 3. replay_history()
 */
void undo_command(state_t *st) {
  command_t *c = pop_command(st->hs);
  queue_command(st->fq, c);
  replay_history(st);
}
