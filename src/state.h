#ifndef STATE_H
#define STATE_H

#define LINE_LENGTH 80

#include <stdlib.h>
#include "screen.h"
#include "buffer.h"

struct window;

typedef enum MODE {
  NORMAL,
  INSERT,
  VISUAL,
  EX
} MODE;

typedef struct state {
  buffer_t *buf;
  screen_t *scr;

  size_t cx, cy;
  // insert/normal/visual/ex
  MODE mode;
} state_t;

state_t *init_state(const char *filename);

/*
 * Cursor position can be computed from:
 * buf->current_row, buf->current_char, scr->top_row
 */
void update_cursor_position(state_t *st);

void destroy_state(state_t *st);

#endif
