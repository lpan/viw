#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "commands.h"
#include "controller.h"

command_stack_t *init_command_stack(void) {
  command_stack_t *cs = malloc(sizeof(command_stack_t));
  cs->top = NULL;
  cs->bottom = NULL;

  return cs;
}

void destroy_command_stack(command_stack_t *cs) {
  command_t *c = cs->bottom, *tmp;

  while (c) {
    tmp = c;
    c = c->next;
    free(tmp);
  }
  free(cs);
}

command_t *init_command(COMMAND_TYPE t, COMMAND_PAYLOAD p) {
  command_t *c = malloc(sizeof(command_t));
  c->type = t;
  c->payload = p;
  c->next = NULL;
  c->prev = NULL;
  return c;
}

// TODO maybe add a group prop to command?
bool is_nav_command(command_t *c) {
  COMMAND_TYPE t = c->type;
  return t == HANDLE_MOVE || t == HANDLE_MOVE_TO_EDGE;
}

static bool is_mode_change_command(command_t *c) {
  return c->type == HANDLE_MODE_CHANGE;
}

bool is_to_normal_command(command_t *c) {
  return is_mode_change_command(c) && c->payload.m == NORMAL;
}

// commands triggers NORMAL -> INSERT_FRONT or INSERT_BACK
bool is_to_insert_command(command_t *c) {
  COMMAND_PAYLOAD p = c->payload;
  return is_mode_change_command(c) && (p.m == INSERT_FRONT || p.m == INSERT_BACK);
}

// -------------------------
// -- History Stack methods
// -------------------------
command_t *append_command(command_stack_t *cs, command_t *c) {
  // assert valid state
  assert((!cs->top && !cs->bottom) || (cs->top && cs->bottom));

  if (!c) {
    return NULL;
  }

  // when cs is empty
  if (!cs->top && !cs->bottom) {
    cs->top = c;
    cs->bottom = c;
    return c;
  }

  c->prev = cs->top;
  cs->top->next = c;
  cs->top = c;
  return c;
}

command_t *pop_command(command_stack_t *cs) {
  assert((!cs->top && !cs->bottom) || (cs->top && cs->bottom));

  if (!cs->top && !cs->bottom) {
    return NULL;
  }

  command_t *popped = cs->top;

  // if there is only one command in stack
  if (popped->prev == NULL) {
    cs->top = NULL;
    cs->bottom = NULL;
    return popped;
  }

  cs->top = popped->prev;
  popped->prev->next = NULL;

  // clear popped links
  popped->prev = NULL;
  return popped;
}

// not a stack anymore LOL
command_t *shift_command(command_stack_t *cs) {
  assert((!cs->top && !cs->bottom) || (cs->top && cs->bottom));

  if (!cs->top && !cs->bottom) {
    return NULL;
  }

  command_t *popped = cs->bottom;

  // if there is only one command in stack
  if (popped->next == NULL) {
    cs->top = NULL;
    cs->bottom = NULL;
    return popped;
  }

  cs->bottom = popped->next;
  popped->next->prev = NULL;

  // clear popped links
  popped->next = NULL;
  return popped;
}
