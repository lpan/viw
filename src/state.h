#ifndef STATE_H
#define STATE_H

#define LINE_LENGTH 80

#include <stdlib.h>
#include <stdbool.h>
#include <ncurses.h>

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

  struct row *prev;
  struct row *next;
} row_t;

typedef struct state {
  size_t cx;
  size_t cy;

  // insert/normal/visual/ex
  MODE mode;

  // rows
  row_t *current;
  row_t *head;
  row_t *last;

  // each ncurses window represents a line
  WINDOW **rows;
  // display current mode, as well as ex mode commands
  WINDOW *status_bar;

  bool is_dirty;
  size_t num_rows;
  const char *filename;
} state_t;

extern state_t *g_state;

void init_state(const char *filename);

void destroy_state(void);

void append_row(const char *buffer);

void prepend_row(const char *buffer);

void delete_row(void);

void update_row(char c);

void up_row(void);

void down_row(void);

#endif
