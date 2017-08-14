#include <stdlib.h>
#include <assert.h>
#include "commands.h"
#include "controller.h"

command_queue_t *init_command_queue(void) {
  command_queue_t *cq = malloc(sizeof(command_queue_t));
  cq->head = NULL;
  cq->last = NULL;
  cq->current = NULL;

  return cq;
}

void destroy_command_queue(command_queue_t *cq) {
  free(cq);
}

void append_command(command_queue_t *cq, COMMAND_TYPE t, COMMAND_PAYLOAD p) {
  command_t *c = malloc(sizeof(command_t));
  c->type = t;
  c->payload = p;
  c->next = NULL;
  c->prev = NULL;

  assert((!cq->head && !cq->last) || (cq->head && cq->last));

  // when cq is empty
  if (!cq->head && !cq->last) {
    cq->head = c;
    cq->last = c;
    return;
  }

  c->prev = cq->last;
  cq->last->next = c;
  cq->last = c;
}

void dispatch_command(command_t *c, state_t *st) {
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

void replay_til_current(command_queue_t *cq, state_t *st) {
  command_t *c = cq->head;

  while (c) {
    dispatch_command(c, st);

    if (c == cq->current) {
      break;
    }

    c = c->next;
  }
}
