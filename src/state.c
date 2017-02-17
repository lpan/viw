#include <assert.h>
#include <stdlib.h>
#include "state.h"

state_t *g_state;

void init_state(char *filename) {
  g_state = malloc(sizeof(state_t));

  g_state->cx = 0;
  g_state->cy = 0;
  g_state->num_rows = 0;
  g_state->is_dirty = false;
  g_state->filename = filename;
  g_state->current = NULL;
  g_state->head = NULL;
  g_state->last = NULL;
}

void destroy_row(row_t *r) {
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

void insert_row(char *buffer, size_t row_i) {
  // append row one at a time
  assert(row_i < g_state->num_rows + 1);

  row_t *r = malloc(sizeof(row_t));
  r->buffer = buffer;
  r->index = row_i;
  r->next = NULL;
  r->prev = NULL;

  // handle empty file
  if (g_state->num_rows == 0) {
    g_state->head = r;
    g_state->last= r;
    g_state->num_rows ++;
    g_state->current = r;
    return;
  }

  // num_rows >= 1
  if (row_i == 0) {
    r->next = g_state->head;
    g_state->head->prev = r;
    g_state->head = r;
  } else if (row_i == g_state->num_rows) {
    r->prev = g_state->last;
    g_state->last->next = r;
    g_state->last = r;
  } else {
    row_t *prev = g_state->head, *next;

    for (size_t i = 0; i < row_i - 1; i ++) {
      prev = prev->next;
    }
    next = prev->next;

    r->next = next;
    r->prev = prev;
    prev->next = r;
    next->prev = r;
  }

  g_state->num_rows ++;
  g_state->current = r;
}

void delete_row(size_t row_i) {
  assert(row_i < g_state->num_rows);
  row_t *to_delete;

  // we always want one line in buffer
  if (g_state->num_rows == 1) {
    g_state->current->buffer = NULL;
    return;
  }

  // reset current if it is deleted
  if (row_i == g_state->current->index) {
    if (g_state->current->next) {
      g_state->current = g_state->current->next;
    } else {
      g_state->current = g_state->current->prev;
    }
  }

  if (row_i == 0) {
    to_delete = g_state->head;
    g_state->head = g_state->head->next;
    g_state->head->prev = NULL;
  } else if (row_i == g_state->num_rows - 1) {
    to_delete = g_state->last;
    g_state->last = g_state->last->prev;
    g_state->last->next = NULL;
  } else {
    row_t *prev = g_state->head, *next;

    // we are sure that row_i < num_rows - 1
    for (size_t i = 0; i < row_i - 1; i ++) {
      prev = prev->next;
    }
    to_delete = prev->next;

    next = prev->next->next;
    prev->next = next;
    next->prev = prev;
  }

  g_state->num_rows --;
  destroy_row(to_delete);
}
