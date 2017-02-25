#ifndef STATE_H
#define STATE_H

#define LINE_LENGTH 80

#include <stdlib.h>
#include "screen.h"
#include "buffer.h"

struct window;

typedef enum MODE {
  NORMAL,
  INSERT_FRONT,
  INSERT_BACK,
  VISUAL,
  EX
} MODE;

typedef struct state {
  buffer_t *buf;
  screen_t *scr;

  size_t cx, cy;
  size_t top_row;

  // rerender all windows if it is true
  bool to_refresh;

  // support two char commands such as 'gg', 'dd'
  char prev_key;

  // insert/normal/visual/ex
  MODE mode;
} state_t;

state_t *init_state(const char *filename);

void update_state(state_t *st);

void move_cursor(state_t *st, DIRECTION d);

void handle_enter(state_t *st);

/*
 * Joine two lines when backspace at the beginning of the line
 * Otherwise delete char in front of the cursor
 */
void handle_backspace(state_t *st);

void destroy_state(state_t *st);

#endif
