#include <stdlib.h>
#include "state.h"

editor_state *g_state;

void init_state(char *filename) {
  g_state = malloc(sizeof(editor_state));

  g_state->cx = 0;
  g_state->cy = 0;
  g_state->num_rows = 0;
  g_state->rows = NULL;
  g_state->is_dirty = false;
  g_state->filename = filename;
}

void destroy_state(void) {
  for (size_t i = 0; i < g_state->num_rows; i ++) {
    free(g_state->rows[i].buffer);
  }

  free(g_state->rows);
  free(g_state);
}

void add_row(char *buffer, size_t len) {
  int index = g_state->num_rows;

  g_state->num_rows ++;
  g_state->rows = realloc(g_state->rows, g_state->num_rows * sizeof(row));

  g_state->rows[index].index = index;
  g_state->rows[index].buffer = buffer;
  g_state->rows[index].len = len;
}
