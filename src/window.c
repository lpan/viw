#include <ncurses.h>
#include <stdlib.h>

void init_window(void) {
  initscr();
  // raw();
  keypad(stdscr, TRUE);
  noecho();
}

void destroy_window(void) {
  endwin();
}
