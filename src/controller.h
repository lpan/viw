#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <stdlib.h>
#include "state.h"
#include "buffer.h"

void handle_move(state_t *st, DIRECTION d);

void handle_move_to_edge(state_t *st, DIRECTION d);

void handle_insert_char(state_t *st, char c);

void handle_append_row(state_t *st);

void handle_prepend_row(state_t *st);

void handle_delete_char(state_t *st);

void handle_delete_row(state_t *st);

void handle_enter(state_t *st);

void handle_backspace(state_t *st);

void set_prev_key(state_t *st, char c);

void reset_prev_key(state_t *st);

#endif
