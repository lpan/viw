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

  // insert/normal/visual/ex
  MODE mode;
} state_t;

state_t *init_state(const char *filename);

void move_cursor(state_t *st, DIRECTION d);

/*
 * Cursor position can be computed from:
 * buf->current_row, buf->current_char, scr->top_row
 */
void update_cursor_position(state_t *st);

/*
 * Determine current rows to be displayed and update windows <-> rows links
 * Can be computed from:
 * st->cy, buf->current_row, scr->top_window, scr->num_windows
 */
void update_scr_windows(state_t *st);

void insert_char(state_t *st, char c);

void destroy_state(state_t *st);

#endif
