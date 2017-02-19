#ifndef WINDOWS_H
#define WINDOWS_H

#include <ncurses.h>
#include "state.h"

typedef struct window {
  WINDOW *w;
  row_t *r;
} window_t;

// each ncurses window represents a line
extern window_t **g_windows;
// display current mode, as well as ex mode commands
extern window_t *g_status_window;

void init_screen(void);

void destroy_screen(void);

void refresh_window(void);

#endif
