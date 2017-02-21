#ifndef STATE_H
#define STATE_H

#define LINE_LENGTH 80

#include <stdlib.h>
#include <stdbool.h>
#include <ncurses.h>

struct window;

typedef enum MODE {
  NORMAL,
  INSERT,
  VISUAL,
  EX
} MODE;

typedef struct echar {
  char c;
  struct echar *prev;
  struct echar *next;
} echar_t;

typedef struct row {
  echar_t *current;
  echar_t *head;
  echar_t *last;
  size_t line_size;

  struct window *win;

  struct row *prev;
  struct row *next;
} row_t;

typedef struct state {
  // r_cy is the relative y coordinate
  size_t cx, cy, r_cy;
  size_t t_cx, t_cy;

  // insert/normal/visual/ex
  MODE mode;

  // rows
  row_t *current;
  row_t *head;
  row_t *last;
  row_t *status;

  bool is_dirty;
  size_t num_rows;
  const char *filename;
} state_t;

extern state_t *g_state;

void init_state(const char *filename);

void destroy_state(void);

void add_char(row_t *r, char c);

void delete_char(row_t *r);

void backspace_char(row_t *r);

void append_row(const char *buffer);

void prepend_row(const char *buffer);

void delete_row(void);

void update_row(char c);

void clear_row(row_t *r);

row_t *next_row(row_t *r);

row_t *prev_row(row_t *r);

void up_row(void);

void down_row(void);

void left_column(row_t *r);

void right_column(row_t *r);

#endif
