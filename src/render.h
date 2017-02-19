#ifndef RENDER_H
#define RENDER_H

#include <ncurses.h>
#include "state.h"

void render_window(window_t *win, row_t *r);

void initial_render(void);

void rerender(void);

#endif