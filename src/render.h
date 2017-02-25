#ifndef RENDER_H
#define RENDER_H

#include "state.h"

void render_window(window_t *w, size_t padding_front);

void render_windows(state_t *st);

void render_update(state_t *st);

#endif
