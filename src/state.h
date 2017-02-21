#ifndef STATE_H
#define STATE_H

#define LINE_LENGTH 80

#include <stdlib.h>
#include <stdbool.h>
#include <ncurses.h>
#include "buffer.h"

struct window;

typedef enum MODE {
  NORMAL,
  INSERT,
  VISUAL,
  EX
} MODE;

typedef struct state {
  // r_cy is the relative y coordinate
  size_t cx, cy;
  size_t t_cx, t_cy;

  // insert/normal/visual/ex
  MODE mode;
} state_t;

extern state_t *g_state;

state_t *init_state(void);

void destroy_state(state_t *st);

#endif
