#ifndef STATE_H
#define STATE_H

#include <stdlib.h>
#include <stdbool.h>

typedef struct row {
  size_t index;
  char *buffer;
  struct row *prev;
  struct row *next;
} row_t;

typedef struct state {
  size_t cx;
  size_t cy;

  row_t *current;
  row_t *head;
  row_t *last;

  bool is_dirty;
  size_t num_rows;
  const char *filename;
} state_t;

extern state_t *g_state;

void init_state(const char *filename);

void destroy_state(void);

void insert_row(char *buffer, size_t index);

void delete_row(size_t index);

#endif
