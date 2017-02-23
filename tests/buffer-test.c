/*
 * ./tests/file.txt:
 *
 * --start--
 * one
 * two
 * three
 * four
 * five
 * --end--
 *
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../src/buffer.h"

static void print_row(row_t *r) {
  echar_t *c = r->head;
  while (c) {
    printf("%c", c->c);
    c = c->next;
  }
  printf("\n");
}

static void print_buffer(buffer_t *buf) {
  row_t *r = buf->head;
  while (r) {
    print_row(r);
    r = r->next;
  }
}

static int mycmp(row_t *r, const char *str) {
  const size_t s = 100;
  char tmp[s];
  echar_t *c = r->head;

  for (size_t i = 0; c; i++) {
    snprintf(tmp + i, s - i, "%c", c->c);
    c = c->next;
  }

  return strcmp(tmp, str);
}

static int test_chars(const char *filename) {
  buffer_t *buf = init_buffer(filename);

  // start at first line, first char
  assert(mycmp(buf->current, "zero") == 0);
  assert(buf->current->current->c == 'z');

  prepend_char(buf->current, 'm');
  assert(mycmp(buf->current, "mzero") == 0);
  assert(buf->current->current->c == 'm');

  delete_char(buf->current);
  assert(mycmp(buf->current, "zero") == 0);
  assert(buf->current->current->c == 'z');

  append_char(buf->current, 'm');
  assert(mycmp(buf->current, "zmero") == 0);
  assert(buf->current->current->c == 'm');

  delete_char(buf->current);
  assert(mycmp(buf->current, "zero") == 0);
  assert(buf->current->current->c == 'e');

  // delete e
  delete_char(buf->current);
  assert(buf->current->current->c == 'r');
  // delete r
  delete_char(buf->current);
  assert(buf->current->current->c == 'o');
  // delete o
  delete_char(buf->current);
  assert(buf->current->current->c == 'z');

  delete_char(buf->current);
  append_char(buf->current, 'x');
  assert(mycmp(buf->current, "x") == 0);
  assert(buf->current->current->c == 'x');

  destroy_buffer(buf);

  return 0;
}

static int test_rows(const char *filename) {
  buffer_t *buf = init_buffer(filename);

  assert(mycmp(buf->current, "zero") == 0);
  assert(buf->num_rows == 6);

  // prepend at head
  prepend_row(buf, "mr. goose");
  assert(mycmp(buf->current->next, "zero") == 0);
  assert(mycmp(buf->current, "mr. goose") == 0);
  assert(mycmp(buf->head, "mr. goose") == 0);
  assert(buf->num_rows == 7);

  // manually move current to bottom
  buf->current = buf->last;

  // append at last
  append_row(buf, "hehe");
  // make sure it is inserted after the old current
  assert(mycmp(buf->current->prev, "five") == 0);
  assert(mycmp(buf->current, "hehe") == 0);
  assert(mycmp(buf->last, "hehe") == 0);
  assert(buf->num_rows == 8);

  // delete last row (since current is last)
  delete_row(buf);
  // if next is null, we set current to prev
  assert(mycmp(buf->current, "five") == 0);
  assert(buf->num_rows == 7);

  buf->current = buf->head;
  // delete first row
  delete_row(buf);
  // set current to next
  assert(mycmp(buf->current, "zero") == 0);
  assert(buf->num_rows == 6);

  destroy_buffer(buf);

  return 0;
}

int test_move_rows(const char *filename) {
  buffer_t *buf = init_buffer(filename);

  assert(mycmp(buf->current, "zero") == 0);
  assert(buf->num_rows == 6);

  // if reach top do nothing
  move_current(buf, UP);
  assert(mycmp(buf->current, "zero") == 0);
  assert(buf->current_row == 0);

  move_current(buf, DOWN);
  assert(mycmp(buf->current, "one") == 0);
  assert(buf->current_row == 1);

  move_current(buf, DOWN);
  move_current(buf, DOWN);
  move_current(buf, DOWN);
  assert(mycmp(buf->current, "four") == 0);
  assert(buf->current_row == 4);
  move_current(buf, DOWN);
  assert(mycmp(buf->current, "five") == 0);
  assert(buf->current_row == 5);
  move_current(buf, DOWN);
  assert(mycmp(buf->current, "five") == 0);
  assert(buf->current_row == 5);

  destroy_buffer(buf);
  return 0;
}

int test_move_chars(const char *filename) {
  buffer_t *buf = init_buffer(filename);

  assert(buf->current->current->c == 'z');
  assert(buf->current_char == 0);

  move_current(buf, LEFT);
  assert(buf->current->current->c == 'z');
  assert(buf->current_char == 0);

  move_current(buf, RIGHT);
  assert(buf->current->current->c == 'e');
  assert(buf->current_char == 1);
  move_current(buf, RIGHT);
  move_current(buf, RIGHT);
  assert(buf->current->current->c == 'o');
  assert(buf->current_char == 3);
  move_current(buf, RIGHT);
  assert(buf->current->current->c == 'o');
  assert(buf->current_char == 3);

  move_current(buf, LEFT);
  assert(buf->current_char == 2);

  destroy_buffer(buf);
  return 0;
}

int main(void) {
  const char *filename = "./file.txt";

  test_chars(filename);
  test_rows(filename);

  test_move_rows(filename);
  test_move_chars(filename);

  return 0;
}
