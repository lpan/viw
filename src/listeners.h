#ifndef KEYPRESS_H
#define KEYPRESS_H

#define KEY_ESC 27

// keypress listeners
// There has to be exactly one listener running

void start_listener(void);

void start_normal_listener(void);

void start_ex_listener(void);

void start_insert_listener(void);

void start_visual_listener(void);

#endif
