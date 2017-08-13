#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "screen.h"
#include "buffer.h"
#include "state.h"
#include "controller.h"

// -----------
// Actions
// -----------
void move_cursor(state_t *st, DIRECTION d) {
  move_current(st->buf, d);
}

void handle_insert(state_t *st, char c) {
  if (st->mode == INSERT_FRONT) {
    prepend_char(st->buf, c);
  }

  if (st->mode == INSERT_BACK) {
    append_char(st->buf, c);
  }
}

void handle_enter(state_t *st) {
  // Edge case: enter at the end of the line in insert_back mode
  if (st->mode == INSERT_BACK && st->buf->current_char == st->buf->current->line_size - 1) {
    append_char(st->buf, '0');
    split_row(st->buf);
    delete_char(st->buf);
    st->to_refresh = true;
    return;
  }

  // in insert_back, cursor one char to the right of "current"
  // we always want to in insert_back mode when line is empty
  if (st->mode == INSERT_BACK && st->buf->current->line_size != 0) {
    move_current(st->buf, RIGHT);
    st->mode = INSERT_FRONT;
  }

  split_row(st->buf);
  st->to_refresh = true;
}

/*
 * Joine two lines when backspace at the beginning of the line
 * Otherwise delete char in front of the cursor
 */
void handle_backspace(state_t *st) {
  buffer_t *buf = st->buf;
  row_t *r = buf->current;

  if (st->mode == EX) {
    move_current(st->buf, LEFT);
    drop_char(buf->status_row);
  }

  if (st->mode == INSERT_FRONT) {
    if (!r->current->prev) {
      join_row(st->buf);
      st->to_refresh = true;
      return;
    }

    move_current(st->buf, LEFT);
    delete_char(buf);
  }

  if (st->mode == INSERT_BACK) {
    if (!r->current) {
      join_row(st->buf);
      st->to_refresh = true;
      return;
    }

    if (buf->current_char == 0) {
      if (r->line_size == 1) {
        delete_char(st->buf);
        return;
      }

      // back insert mode cant handle this situation
      st->mode = INSERT_FRONT;
      move_current(st->buf, RIGHT);
      handle_backspace(st);
      return;
    }

    delete_char(buf);

    if (r->current->next) {
      move_current(st->buf, LEFT);
    }
  }
}

void set_prev_key(state_t *st, char c) {
  st->prev_key = c;
}

void reset_prev_key(state_t *st) {
  st->prev_key = '\0';
}
