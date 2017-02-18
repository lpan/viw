#include <ncurses.h>
#include "state.h"

void start_normal_listener(void) {
  while (g_state->mode == NORMAL) {
    int ch = getch();
    switch (ch) {
      case KEY_LEFT:
        printw("left\n");
        break;
      case KEY_RIGHT:
        printw("right\n");
        break;
      default:
        printw("%c\n", ch);
        break;
    }
    refresh();
  }
}

void start_insert_listener(void) {
}

void start_visual_listener(void) {
}
