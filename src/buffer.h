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
} buffer_t;

buffer_t *init_buffer(const char *filename);

/*
 * Primitive add char
 */
void add_char(row_t *r, char c);

/*
 * Append char to line then set the current char to new char
 */
void append_char(buffer_t *buf, char c);

/*
 * Prepend char to line then set the current char to new char
 */
void prepend_char(buffer_t *buf, char c);

/*
 * Delete current char and set 'current' to point to next char
 * if only NULL char is present, do nothing
 * if next char is NULL, point to the previous char
 */
void delete_char(buffer_t *buf);

/*
 * Add an empty line below the current line and set current to the new line
 */
void append_row(buffer_t *buf, const char *line);

/*
 * Add an empty line above the current line and set current to the new line
 */
void prepend_row(buffer_t *buf, const char *line);

/*
 * Move the cursor up/down/left/right
 * Do nothing if it reaches the end
 */
void move_current(buffer_t *buf, DIRECTION d);

/*
 * Move cursor all the way to the right
 */
void to_right(buffer_t *buf);

/*
 * Move cursor all the way to the left
 */
void to_left(buffer_t *buf);

/*
 * Make the row empty
 */
void clear_row(row_t *r);

/*
 * Delete current row and reset 'current' to point to the next row.
 * If next is null then set it to previous
 * If there is only one row left, we simply "clear" the row
 */
void delete_row(buffer_t *buf);

void destroy_buffer(buffer_t *buf);

#endif
