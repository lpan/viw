#ifndef STATE_H
#define STATE_H

#include <stdlib.h>
#include <stdbool.h>

typedef struct row {
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

void append_row(char *buffer);

void prepend_row(char *buffer);

void delete_row(void);

void update_row(char *new_buffer);

void up_row(void);

void down_row(void);

#endif
