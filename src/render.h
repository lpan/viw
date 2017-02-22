#ifndef RENDER_H
#define RENDER_H

#include <ncurses.h>
#include "screen.h"
#include "state.h"

void render_window(window_t *win, row_t *r);

void render_some(row_t *(*proceed) (row_t *), size_t n);

void render_all(row_t *(*proceed) (row_t *));

void initial_render(state_t *st, buffer_t *buf);

void rerender(state_t *st);

#endif
