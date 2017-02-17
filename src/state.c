#include <assert.h>
#include <stdlib.h>
#include "state.h"

state_t *g_state;

void init_state(const char *filename, size_t max_rows) {
  g_state = malloc(sizeof(state_t));

  g_state->cx = 0;
  g_state->cy = 0;

  g_state->is_dirty = false;
  g_state->filename = filename;

  g_state->num_rows = 0;
  g_state->head = NULL;
  g_state->last = NULL;
  g_state->current = NULL;

  g_state->top = NULL;
  g_state->max_rows = max_rows;
}

static void destroy_row(row_t *r) {
  free(r->buffer);
  free(r);
  r = NULL;
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

// insert row after current
void append_row(char *buffer) {
  row_t *r = malloc(sizeof(row_t));
  row_t *prev = NULL;
  row_t *next = NULL;

  r->buffer = buffer;
  r->next = NULL;
  r->prev = NULL;

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
  row_t *r = malloc(sizeof(row_t));
  row_t *prev = NULL;
  row_t *next = NULL;

  r->buffer = buffer;
  r->next = NULL;
  r->prev = NULL;

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
    g_state->current->buffer = NULL;
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

// update current row
void update_row(char *new_buffer) {
  g_state->current->buffer = new_buffer;
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
