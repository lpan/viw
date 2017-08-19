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

// ------------------------------
// Mutations modeled as commands
// ------------------------------
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
}

void handle_prepend_row(state_t *st) {
  prepend_row(st->buf, NULL);
  st->to_refresh = true;
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

void handle_mode_change(state_t *st, MODE m) {
  st->buf->mode = m;
}

// ---------
// Helpers
// ---------
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
    case HANDLE_MODE_CHANGE:
      handle_mode_change(st, c->payload.m);
      break;
  }
}

static void replay_history(state_t *st) {
  command_stack_t *hs = st->hs;
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

  // FIXME kill this when implemented redo
  destroy_command_stack(st->rs);
  st->rs = init_command_stack();
}

/*
 * LOL imperative programming sucks
 *
 * 1. pop history stack and push the result to the redo stack
 * 2. if the command is a nav command, keep popping until a mutation command is
 *    encountered (go to step 5) or the stack is empty (go to step 4).
 * 3. if the command is a mode changing command that changes the buffer from
 *    insert mode to normal mode, keep popping until we hit another mode
 *    changing command (as we want to undo all changes performed in insert mode
 *    at once)
 * 4. if the command is empty, we know that all commands that were previously in
 *    the history stack are nav commands, we want to add them back and abort the
 *    operation.
 * 5. replay the resulting history stack against the initial buffer.
 */
void undo_command(state_t *st) {
  // 1
  command_t *c = pop_command(st->hs);
  append_command(st->rs, c);

  // 2
  while (c && is_nav_command(c)) {
    c = pop_command(st->hs);
    append_command(st->rs, c);
  }

  // 3
  if (c && is_to_normal_command(c)) {
    // we are confident that there is a to-insert-command somewhere in the
    // history stack
    while (!is_to_insert_command(c)) {
      c = pop_command(st->hs);
      append_command(st->rs, c);
    }
  }

  // 4
  if (!c) {
    c = pop_command(st->rs);
    append_command(st->hs, c);
    while (c && is_nav_command(c)) {
      c = pop_command(st->rs);
      append_command(st->hs, c);
    }

    if (!c || !is_nav_command(c)) {
      return;
    }
  }

  // 5
  replay_history(st);
  st->to_refresh = true;
}
