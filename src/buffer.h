#ifndef BUFFER_H
#define BUFFER_H

#include <stdbool.h>
#include <stdlib.h>

typedef enum DIRECTION {
  UP,
  DOWN,
  RIGHT,
  LEFT
} DIRECTION;

typedef enum MODE {
  NORMAL,
  INSERT_FRONT,
  INSERT_BACK,
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
  bool is_dirty;

  struct row *prev;
  struct row *next;
} row_t;

typedef struct buffer {
  row_t *current;
  row_t *head;
  row_t *last;

  // buffer for the status line
  row_t *status_row;

  // total number of rows out there
  size_t num_rows;

  // current char/row (x, y) index
  size_t current_char;
  size_t current_row;

  bool is_dirty;
  const char *filename;

  // insert/normal/visual/ex
  MODE mode;
} buffer_t;

buffer_t *init_buffer(const char *filename);

void add_char(row_t *r, char c);

void drop_char(row_t *r);

void append_char(buffer_t *buf, char c);

void prepend_char(buffer_t *buf, char c);

void delete_char(buffer_t *buf);

void append_row(buffer_t *buf, const char *line);

void prepend_row(buffer_t *buf, const char *line);

void join_row(buffer_t *buf);

void split_row(buffer_t *buf);

void move_current(buffer_t *buf, DIRECTION d);

void to_right(buffer_t *buf);

void to_left(buffer_t *buf);

void to_top(buffer_t *buf);

void to_bottom(buffer_t *buf);

void clear_row(row_t *r);

void delete_row(buffer_t *buf);

void destroy_buffer(buffer_t *buf);

#endif
