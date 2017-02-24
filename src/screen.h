#ifndef SCREEN_H
#define SCREEN_H

#include <ncurses.h>

typedef struct row row_t;

typedef struct window {
  WINDOW *w;
  row_t *r;
} window_t;

typedef struct screen {
  // total number of non status windows displayed
  size_t num_windows;

  // each ncurses window represents a line
  window_t **windows;
  // display current mode, as well as ex mode commands
  window_t *status_window;
} screen_t;

screen_t *init_screen(size_t term_height);

void destroy_screen(screen_t *scr);

#endif
