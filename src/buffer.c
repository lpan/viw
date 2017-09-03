#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include "buffer.h"

#ifdef _WIN32
int getline(char **lp, size_t *n, FILE *fp) {
  char buf[BUFSIZ];
  char *p;
  size_t len;

  if (lp == NULL || n == NULL) {
    errno = EINVAL;
    return -1;
  }

  if (ferror (fp) || feof(fp))
    return -1;

  buf[0] = '\0';
  fgets(buf, BUFSIZ, fp);
  p = strchr(buf, '\n');
  if (p) *p = '\0';
  len = strlen(buf);

  if (len < BUFSIZ-1) {
    p = realloc(*lp, BUFSIZ);
    if (p == NULL) return -1;
    *lp = p;
    *n = BUFSIZ;
  }

  strcpy(*lp, buf);
  return len;
}
#endif

/*
 * Read file to buffer
 */
static void read_file(buffer_t *buf, const char *filename) {
  // read file to memory
  FILE *fp = fopen(filename, "r");
  if (fp) {
    char *line = NULL;
    size_t len = 0;

    for (size_t i = 0; getline(&line, &len, fp) != -1; i++) {
      // trim newline char
      line[strcspn(line, "\n")] = 0;

      append_row(buf, line);
    }

    free(line);
    line = NULL;
    fclose(fp);
  }

  // insert empty row if file is empty or doesn't exist
  if (!buf->current) {
    append_row(buf, NULL);
  }
}

void add_char(row_t *r, char c) {
  echar_t *ec = malloc(sizeof(echar_t));
  echar_t *next = NULL;
  echar_t *prev = NULL;

  ec->c = c;
  ec->prev = NULL;
  ec->next = NULL;

  if (r->line_size == 0) {
    r->head = ec;
    r->last = ec;
    r->current = ec;
    r->line_size ++;
    r->is_dirty = true;
    return;
  }

  if (r->current == r->last) {
    prev = r->current;
    prev->next = ec;
    r->last = ec;
  } else {
    prev = r->current;
    next = r->current->next;
    prev->next = ec;
    next->prev = ec;
  }

  ec->next = next;
  ec->prev = prev;

  r->is_dirty = true;
  r->line_size ++;
  r->current = ec;
}

/*
 * Init a row with a NULL char at the beginning
 */
row_t *init_row(const char *line) {
  row_t *r = malloc(sizeof(row_t));

  r->next = NULL;
  r->prev = NULL;
  r->line_size = 0;
  r->is_dirty = true;

  r->head = NULL;
  r->last = NULL;
  r->current = NULL;

  if (!line) {
    return r;
  }

  for (size_t i = 0; i < strlen(line); i ++) {
    add_char(r, line[i]);
  }

  // set current to be the first char
  r->current = r->head;
  return r;
}

void destroy_row(row_t *r) {
  echar_t *c = r->head, *tmp;
  while (c) {
    tmp = c;
    c = c->next;
    free(tmp);
  }
  free(r);
}

buffer_t *init_buffer(const char *filename) {
  buffer_t *buf = malloc(sizeof(buffer_t));

  buf->num_rows = 0;
  buf->head = NULL;
  buf->last = NULL;
  buf->current = NULL;

  buf->is_dirty = false;
  buf->filename = filename;

  buf->mode = NORMAL;

  read_file(buf, filename);

  // we want to start at the top when user opens up a new file
  buf->current = buf->head;
  // Set current to the first char of line
  buf->current->current = buf->current->head;

  buf->current_char = 0;
  buf->current_row = 0;

  // go to the first non-empty character
  if (buf->current->line_size > 1) {
    while (buf->current->current->c == ' ' && buf->current->current->next) {
      buf->current->current = buf->current->current->next;
      buf->current_char ++;
    }
  }

  return buf;
}

/*
 * Append char to line then set the current char to new char
 */
void append_char(buffer_t *buf, char c) {
  if (buf->current->line_size != 0) {
    buf->current_char ++;
  }

  add_char(buf->current, c);
}

/*
 * Prepend char to line then set the current char to new char
 */
void prepend_char(buffer_t *buf, char c) {
  row_t *r = buf->current;

  assert(r->line_size > 0);

  // insert at head
  if (r->head && r->current == r->head) {
    echar_t *ec = malloc(sizeof(echar_t));
    ec->c = c;
    ec->prev = NULL;
    ec->next = r->head;

    r->head->prev = ec;
    r->head = ec;
    r->current = ec->next;

    buf->current_char ++;
    r->is_dirty = true;
    r->line_size ++;
    return;
  }

  r->current = r->current->prev;
  append_char(buf, c);
  r->current = r->current->next;
}

void drop_char(row_t *r) {
  if (r->line_size == 0) {
    return;
  }

  echar_t *to_delete = r->current;

  if (r->line_size == 1) {
    to_delete = r->current;
    r->current = NULL;
    r->head = NULL;
    r->last = NULL;

    r->line_size --;
    r->is_dirty = true;

    free(to_delete);
    return;
  }

  if (to_delete == r->head) {
    r->current = to_delete->next;
    r->head = r->current;
    r->head->prev = NULL;
  } else if (to_delete == r->last) {
    r->current = to_delete->prev;
    r->last = r->current;
    r->last->next = NULL;
  } else {
    r->current = to_delete->next;
    r->current->prev = to_delete->prev;
    to_delete->prev->next = r->current;
  }

  r->line_size --;
  r->is_dirty = true;

  free(to_delete);
}

/*
 * Delete current char and set 'current' to point to next char
 * if only NULL char is present, do nothing
 * if next char is NULL, point to the previous char
 */
void delete_char(buffer_t *buf) {
  row_t *r = buf->current;

  if (r->line_size > 1 && r->current && r->current == r->last) {
    buf->current_char --;
  }

  drop_char(buf->current);
}

/*
 * Add an empty line below the current line and set current to the new line
 */
void append_row(buffer_t *buf, const char *line) {
  row_t *r = init_row(line);
  row_t *prev = NULL;
  row_t *next = NULL;

  if (buf->num_rows == 0) {
    buf->head = r;
    buf->last= r;
  } else if (buf->current == buf->last) {
    prev = buf->current;
    prev->next = r;
    buf->last = r;
  } else {
    prev = buf->current;
    next = buf->current->next;
    prev->next = r;
    next->prev = r;
  }

  r->next = next;
  r->prev = prev;

  buf->current_char = 0;
  buf->current_row ++;
  buf->num_rows ++;
  buf->current = r;
}

/*
 * Add an empty line above the current line and set current to the new line
 */
void prepend_row(buffer_t *buf, const char *line) {
  // insert at head
  if (buf->head && buf->current == buf->head) {
    row_t *r = init_row(line);
    r->prev = NULL;
    r->next = buf->head;

    buf->head->prev = r;
    buf->head = r;
    buf->current = r;

    buf->current_char = 0;
    buf->current_row = 0;
    buf->num_rows ++;
    return;
  }

  move_current(buf, UP);
  append_row(buf, line);
}

static void adjust_current_char(buffer_t *buf) {
  row_t *r = buf->current;
  echar_t *ec = r->head;

  if (!ec) {
    buf->current_char = 0;
    return;
  }

  for (size_t i = 0; i < buf->current_char; i ++) {
    if (!ec->next) {
      buf->current_char = i;
      break;
    }

    ec = ec->next;
  }

  r->current = ec;
}

static void move_up(buffer_t *buf) {
  if (buf->current->prev) {
    buf->current = buf->current->prev;
    buf->current_row --;
    adjust_current_char(buf);
  }
}

static void move_down(buffer_t *buf) {
  if (buf->current->next) {
    buf->current = buf->current->next;
    buf->current_row ++;
    adjust_current_char(buf);
  }
}

static void move_left(buffer_t *buf) {
  row_t *r = buf->current;
  if (r->current && r->current->prev) {
    r->current = r->current->prev;
    buf->current_char --;
  }
}

static void move_right(buffer_t *buf) {
  row_t *r = buf->current;
  if (r->current && r->current->next) {
    r->current = r->current->next;
    buf->current_char ++;
  }
}

/*
 * Move the cursor up/down/left/right
 * Do nothing if it reaches the end
 */
void move_current(buffer_t *buf, DIRECTION d) {
  switch (d) {
    case UP:
      move_up(buf);
      break;
    case DOWN:
      move_down(buf);
      break;
    case LEFT:
      move_left(buf);
      break;
    case RIGHT:
      move_right(buf);
      break;
    default:
      break;
  }
}

/*
 * Move cursor all the way to the right
 */
void to_right(buffer_t *buf) {
  for (size_t i = buf->current_char; i < buf->current->line_size; i ++) {
    move_current(buf, RIGHT);
  }
}

void to_left(buffer_t *buf) {
  size_t cur = buf->current_char;
  for (size_t i = 0; i < cur; i ++) {
    move_current(buf, LEFT);
  }
}

void to_top(buffer_t *buf) {
  size_t cur = buf->current_row;
  for (size_t i = 0; i < cur; i ++) {
    move_current(buf, UP);
  }
}

void to_bottom(buffer_t *buf) {
  for (size_t i = buf->current_row; i < buf->num_rows; i ++) {
    move_current(buf, DOWN);
  }
}

/*
 * Join current row and the row above
 */
void join_row(buffer_t *buf) {
  // do nothing on top line
  if (!buf->current->prev) {
    return;
  }

  row_t *src = buf->current;
  row_t *dest = src->prev;

  if (dest->line_size == 0) {
    move_current(buf, UP);
    delete_row(buf);

    return;
  }

  if (src->line_size == 0) {
    delete_row(buf);
    buf->current = dest;
    dest->current = dest->last;
    buf->current_row --;
    buf->current_char = dest->line_size - 1;

    return;
  }

  buf->current_char = dest->line_size;

  dest->line_size += src->line_size;
  dest->last->next = src->head;
  src->head->prev = dest->last;

  dest->current = src->head;
  dest->last = src->last;

  src->head = NULL;
  src->last = NULL;

  delete_row(buf);
  move_current(buf, UP);
}

/*
 * When user press enter in insert mode, we split the line into two
 * Current char becomes the head of the new line
 */
void split_row(buffer_t *buf) {
  row_t *src = buf->current;
  size_t current_char = buf->current_char;

  append_row(buf, NULL);
  row_t *dest = buf->current;

  if (src->line_size == 0) {
    return;
  }

  dest->line_size = src->line_size - current_char;
  src->line_size = current_char;

  dest->head = src->current;
  dest->last = src->last;

  src->last = src->current->prev;

  // enter at the very end of the line
  if (dest->head) {
    dest->head->prev = NULL;
  }

  // enter at the beginning of the line
  if (src->last) {
    src->last->next = NULL;
  } else {
    // src->last is NULL implies line is empty
    src->head = NULL;
  }

  dest->current = dest->head;
  buf->current_char = 0;
}

void clear_row(row_t *r) {
  echar_t *ec = r->head;
  while (ec) {
    echar_t *tmp = ec;
    ec = ec->next;
    free(tmp);
  }

  r->last = r->head;
  r->current = r->head;
  r->line_size = 0;
}

/*
 * Delete current row and reset 'current' to point to the next row.
 * If next is null then set it to previous
 * If there is only one row left, we simply "clear" the row
 */
void delete_row(buffer_t *buf) {
  row_t *to_delete = buf->current;

  // there is at least one row in buffer
  if (buf->num_rows == 1) {
    clear_row(buf->current);
    return;
  }

  if (to_delete == buf->head) {
    buf->current = to_delete->next;
    buf->current->prev = NULL;
    buf->head = buf->current;
  } else if (to_delete == buf->last) {
    buf->current = to_delete->prev;
    buf->current->next = NULL;
    buf->last = buf->current;
    buf->current_row --;
  } else {
    to_delete->prev->next = to_delete->next;
    to_delete->next->prev = to_delete->prev;
    buf->current = to_delete->next;
  }

  buf->num_rows --;
  destroy_row(to_delete);
}

void destroy_buffer(buffer_t *buf) {
  row_t *r = buf->head, *tmp;

  while (r) {
    tmp = r;
    r = r->next;
    destroy_row(tmp);
  }

  free(buf);
}
