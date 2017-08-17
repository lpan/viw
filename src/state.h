#ifndef STATE_H
#define STATE_H

#define LINE_LENGTH 80

#include <stdlib.h>
#include "screen.h"
#include "buffer.h"
#include "error.h"

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

  // reserve some space in front of each line to display line number
  // This property depends on the # of digits of buf->num_rows
  size_t padding_front;

  // rerender all windows if it is true
  bool to_refresh;

  // prevent app from closing 
  bool to_save;

  // if error_code != 0, status line will render error message 
  int error_code;

  // support two char commands such as 'gg', 'dd'
  char prev_key;

  // insert/normal/visual/ex
  MODE mode;
} state_t;

state_t *init_state(const char *filename);

void destroy_state(state_t *st);

void update_state(state_t *st);

#endif
