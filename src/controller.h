#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <stdlib.h>
#include "state.h"
#include "buffer.h"
#include "commands.h"

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

void apply_command(state_t *st, COMMAND_TYPE t, COMMAND_PAYLOAD p);

void undo_command(state_t *st);

void replay_history(state_t *st);

#endif
