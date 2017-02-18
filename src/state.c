#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "state.h"

state_t *g_state;

void init_state(const char *filename) {
  g_state = malloc(sizeof(state_t));

  g_state->cx = 0;
  g_state->cy = 0;

  g_state->is_dirty = false;
  g_state->filename = filename;

  g_state->num_rows = 0;
  g_state->head = NULL;
  g_state->last = NULL;
  g_state->current = NULL;

  g_state->mode = NORMAL;
}

static void destroy_row(row_t *r) {
  echar_t *c = r->head, *tmp;
  while (c) {
    tmp = c;
    c = c->next;
    free(tmp);
  }
  free(r);
}

void destroy_state(void) {
  row_t *r = g_state->head, *tmp;
  while (r) {
    tmp = r;
    r = r->next;
    destroy_row(tmp);
  }

  free(g_state);
  g_state = NULL;
}

static void append_char(row_t *r, char c) {
  echar_t *ec = malloc(sizeof(echar_t));
  echar_t *next = NULL;
  echar_t *prev = NULL;

  ec->c = c;
  ec->prev = NULL;
  ec->next = NULL;

  if (r->line_size == 0) {
    r->head = ec;
    r->last= ec;
  } else if (r->current == r->last) {
    prev = r->current;
    prev->next = ec;
    r->last = ec;
  } else {
    prev = r->current;
    next = r->current->next;
    prev->next = ec;
    next->prev = ec;
  }

  ec->next = next;
  ec->prev = prev;

  r->line_size ++;
  r->current = ec;
}

static row_t *init_row(char *buffer) {
  row_t *r = malloc(sizeof(row_t));
  r->next = NULL;
  r->prev = NULL;
  r->line_size = 0;

  if (!buffer) {
    r->current = NULL;
    r->head = NULL;
    r->last = NULL;
    return r;
  }

  for (size_t i = 0; i < strlen(buffer); i ++) {
    append_char(r, buffer[i]);
  }
  return r;
}

// insert row after current
void append_row(char *buffer) {
  row_t *r = init_row(buffer);
  row_t *prev = NULL;
  row_t *next = NULL;

  if (g_state->num_rows == 0) {
    g_state->head = r;
    g_state->last= r;
  } else if (g_state->current == g_state->last) {
    prev = g_state->current;
    prev->next = r;
    g_state->last = r;
  } else {
    prev = g_state->current;
    next = g_state->current->next;
    prev->next = r;
    next->prev = r;
  }

  r->next = next;
  r->prev = prev;

  g_state->num_rows ++;
  g_state->current = r;
}

// insert row before current
void prepend_row(char *buffer) {
  row_t *r = init_row(buffer);
  row_t *prev = NULL;
  row_t *next = NULL;

  if (g_state->num_rows == 0) {
    g_state->head = r;
    g_state->last= r;
  } else if (g_state->current == g_state->head) {
    next = g_state->current;
    next->prev = r;
    g_state->head = r;
  } else {
    prev = g_state->current->prev;
    next = g_state->current;
    prev->next = r;
    next->prev = r;
  }

  r->next = next;
  r->prev = prev;

  g_state->num_rows ++;
  g_state->current = r;
}

// delete current row
void delete_row(void) {
  assert(g_state->num_rows > 0);
  row_t *to_delete = g_state->current;

  // we always want one line in buffer
  if (g_state->num_rows == 1) {
    to_delete = g_state->current;
    g_state->current = init_row(NULL);
    destroy_row(to_delete);
    return;
  }

  if (to_delete == g_state->head) {
    g_state->current = to_delete->next;
    g_state->current->prev = NULL;
    g_state->head = g_state->current;;
  } else if (to_delete == g_state->last) {
    g_state->current = to_delete->prev;
    g_state->current->next = NULL;
    g_state->last = g_state->current;;
  } else {
    to_delete->prev->next = to_delete->next;
    to_delete->next->prev = to_delete->prev;
    g_state->current = to_delete->next;
  }

  g_state->num_rows --;
  destroy_row(to_delete);
}

void update_row(char c) {
}

// move up
void up_row(void) {
  if (g_state->current->prev) {
    g_state->current = g_state->current->prev;
    g_state->cy --;
  }
}

// move down
void down_row(void) {
  if (g_state->current->next) {
    g_state->current = g_state->current->next;
    g_state->cy ++;
  }
}
