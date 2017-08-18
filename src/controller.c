#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "screen.h"
#include "buffer.h"
#include "state.h"
#include "commands.h"
#include "controller.h"

// -----------
// Mutations
// -----------
void handle_move(state_t *st, DIRECTION d) {
  move_current(st->buf, d);
}

void handle_move_to_edge(state_t *st, DIRECTION d) {
  switch (d) {
    case UP:
      to_top(st->buf);
      st->to_refresh = true;
      break;
    case DOWN:
      to_bottom(st->buf);
      st->to_refresh = true;
      break;
    case LEFT:
      to_left(st->buf);
      break;
    case RIGHT:
      to_right(st->buf);
      break;
    default:
      break;
  }
}

void handle_insert_char(state_t *st, char c) {
  if (st->buf->mode == INSERT_FRONT) {
    prepend_char(st->buf, c);
  }

  if (st->buf->mode == INSERT_BACK) {
    append_char(st->buf, c);
  }
}

void handle_append_row(state_t *st) {
  append_row(st->buf, NULL);
  st->to_refresh = true;
  st->buf->mode = INSERT_BACK;
}

void handle_prepend_row(state_t *st) {
  prepend_row(st->buf, NULL);
  st->to_refresh = true;
  st->buf->mode = INSERT_BACK;
}

void handle_delete_char(state_t *st) {
  delete_char(st->buf);
}

void handle_delete_row(state_t *st) {
  delete_row(st->buf);
  st->to_refresh = true;
}

void handle_enter(state_t *st) {
  // Edge case: enter at the end of the line in insert_back mode
  if (st->buf->mode == INSERT_BACK && st->buf->current_char == st->buf->current->line_size - 1) {
    append_char(st->buf, '0');
    split_row(st->buf);
    delete_char(st->buf);
    st->to_refresh = true;
    return;
  }

  // in insert_back, cursor one char to the right of "current"
  // we always want to in insert_back mode when line is empty
  if (st->buf->mode == INSERT_BACK && st->buf->current->line_size != 0) {
    move_current(st->buf, RIGHT);
    st->buf->mode = INSERT_FRONT;
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

  if (st->buf->mode == EX) {
    move_current(st->buf, LEFT);
    drop_char(st->status_row);
  }

  if (st->buf->mode == INSERT_FRONT) {
    if (!r->current->prev) {
      join_row(st->buf);
      st->to_refresh = true;
      return;
    }

    move_current(st->buf, LEFT);
    delete_char(buf);
  }

  if (st->buf->mode == INSERT_BACK) {
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
      st->buf->mode = INSERT_FRONT;
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

// ------------
// Undo & Redo
// ------------
static void dispatch_command(state_t *st, command_t *c) {
  switch (c->type) {
    case HANDLE_MOVE:
      handle_move(st, c->payload.d);
      break;
    case HANDLE_MOVE_TO_EDGE:
      handle_move_to_edge(st, c->payload.d);
      break;
    case HANDLE_INSERT_CHAR:
      handle_insert_char(st, c->payload.c);
      break;
    case HANDLE_APPEND_ROW:
      handle_append_row(st);
      break;
    case HANDLE_PREPEND_ROW:
      handle_prepend_row(st);
      break;
    case HANDLE_DELETE_CHAR:
      handle_delete_char(st);
      break;
    case HANDLE_DELETE_ROW:
      handle_delete_row(st);
      break;
    case HANDLE_ENTER:
      handle_enter(st);
      break;
    case HANDLE_BACKSPACE:
      handle_backspace(st);
      break;
  }
}

void replay_history(state_t *st) {
  history_stack_t *hs = st->hs;
  command_t *c = hs->bottom;

  // TODO copy init buffer but not reconstruct it
  buffer_t *old_buf = st->buf;
  st->buf = init_buffer(old_buf->filename);
  destroy_buffer(old_buf);

  while (c) {
    dispatch_command(st, c);
    c = c->next;
  }
}

/*
 * 1. create command
 * 2. push it to stack
 * 3. execute the command against state
 */
void apply_command(state_t *st, COMMAND_TYPE t, COMMAND_PAYLOAD p) {
  command_t *c = init_command(t, p);
  append_command(st->hs, c);
  dispatch_command(st, c);
}

/*
 * 1. pop history stack
 * 2. push the result from 1. to future queue
 * 3. replay_history()
 */
void undo_command(state_t *st) {
  command_t *c = pop_command(st->hs);
  queue_command(st->fq, c);
  replay_history(st);
}
