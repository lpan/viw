#ifndef STATE_H
#define STATE_H

#include <stdlib.h>
#include <stdbool.h>

typedef struct {
  size_t index;
  size_t len;
  char *buffer;
} row;

typedef struct {
  size_t cx;
  size_t cy;
  bool is_dirty;
  char *filename;
  row *rows;
  size_t num_rows;
} editor_state;

extern editor_state *g_state;

void init_state(char *filename);

void destroy_state(void);

void add_row(char *buffer, size_t len);

#endif
