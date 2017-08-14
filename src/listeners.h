#ifndef KEYPRESS_H
#define KEYPRESS_H

#ifdef __APPLE__
    #undef KEY_BACKSPACE
    #define KEY_BACKSPACE 127
#endif 

#define KEY_ESC 27

#include "controller.h"
#include "buffer.h"

// keypress listeners
// There has to be exactly one listener running

void start_listener(state_t *st);

void start_normal_listener(state_t *st);

void start_ex_listener(state_t *st);

void start_insert_listener(state_t *st);

#endif
