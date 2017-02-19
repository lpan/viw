#ifndef WINDOWS_H
#define WINDOWS_H

#include <ncurses.h>
#include "state.h"

typedef struct window {
  WINDOW *w;
  row_t *r;
} window_t;

typedef struct screen {
  // total number of non status windows displayed
  size_t num_windows;
  // index of the first window displayed
  size_t top_window;

  // each ncurses window represents a line
  window_t **windows;
  // display current mode, as well as ex mode commands
  window_t *status_window;
} screen_t;

extern screen_t *g_screen;

void init_screen(void);

void destroy_screen(void);

void refresh_window(void);

#endif
