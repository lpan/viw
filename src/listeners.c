#include <ncurses.h>
#include "render.h"
#include "state.h"

void start_normal_listener(void) {
  while (g_state->mode == NORMAL) {
    int ch = getch();
    switch (ch) {
      case 'j':
        down_row();
        break;
      case 'k':
        up_row();
        break;
      case 'h':
        left_column(g_state->current);
        break;
      case 'l':
        right_column(g_state->current);
        break;
      default:
        break;
    }

    rerender();
  }
}

void start_insert_listener(void) {
}

void start_visual_listener(void) {
}
