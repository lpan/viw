#ifndef KEYPRESS_H
#define KEYPRESS_H

#define KEY_ESC 27

#include "state.h"
#include "buffer.h"

// keypress listeners
// There has to be exactly one listener running

void start_listener(state_t *st);

void start_normal_listener(state_t *st);

void start_ex_listener(state_t *st);

void start_insert_listener(state_t *st);

#endif
