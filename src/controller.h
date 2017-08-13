#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <stdlib.h>
#include "state.h"
#include "buffer.h"

void move_cursor(state_t *st, DIRECTION d);

void handle_insert(state_t *st, char c);

void handle_enter(state_t *st);

void handle_backspace(state_t *st);

void set_prev_key(state_t *st, char c);

void reset_prev_key(state_t *st);

#endif
