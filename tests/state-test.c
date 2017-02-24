/*
 * Integration test for state, buffer and screen
 */
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../src/state.h"

int test_update_cursor(const char *filename) {
  // cursor moved to an empty line
  state_t *st = init_state(filename);
  assert(st->cx == 0 && st->cy == 0);

  update_cursor_position(st);
  assert(st->cx == 0 && st->cy == 0);

  destroy_state(st);
  return 0;
}

int test_update_display(const char *filename) {
  state_t *st = init_state(filename);
  window_t **windows = st->scr->windows;
  row_t *head = st->buf->head;

  assert(st->scr->num_windows == (size_t) LINES - 1);

  update_display(st);
  for (size_t i = 0; i < st->scr->num_windows; i ++) {
    if (head) {
      assert(windows[i]->r == head);
    } else {
      assert(windows[i]->r == NULL);
    }
  }

  destroy_state(st);
  return 0;
}

int main(void) {
  const char *filename = "./file.txt";

  // ncurses stuff
  initscr();
  raw();
  keypad(stdscr, TRUE);
  noecho();

  test_update_cursor(filename);
  test_update_display(filename);

  endwin();
  return 0;
}
