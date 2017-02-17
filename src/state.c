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
}

void destroy_state(void) {
  row_t *r = g_state->head, *tmp;
  while (r) {
    tmp = r;
    r = r->next;
    free(tmp);
  }

  free(g_state);
}

void insert_row(char *buffer, size_t row_i) {
  // append newline one at a time
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
}
