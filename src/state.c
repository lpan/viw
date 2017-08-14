#include "state.h"
#include <string.h>

state_t *init_state(const char *filename) {
  state_t *st = malloc(sizeof(state_t));

  st->mode = NORMAL;
  st->cx = 0;
  st->cy = 0;
  st->top_row = 0;

  st->to_refresh = true;

  st->buf = init_buffer(filename);
  st->scr = init_screen(LINES);

  st->prev_key = '\0';

  update_state(st);

  return st;
}

void destroy_state(state_t *st) {
  destroy_buffer(st->buf);
  destroy_screen(st->scr);
  free(st);
}

// -----------
// Getters
// -----------
static void update_top_row(state_t *st) {
  size_t current_row = st->buf->current_row;
  size_t num_windows = st->scr->num_windows;

  // when user is scrolling down
  if (current_row >= st->top_row + num_windows) {
    st->top_row = current_row - num_windows + 1;
    st->to_refresh = true;
  }

  // when user is scrolling up
  if (current_row < st->top_row) {
    st->top_row = current_row;
    st->to_refresh = true;
  }
}

/*
 * compute the space reserved for line number
 */
static void update_padding_front(state_t *st) {
  size_t max_row = st->buf->num_rows;
  size_t num_digits = 0;

  while (max_row > 0) {
    max_row /= 10;
    num_digits ++;
  }

  st->padding_front = num_digits + 1;
}

/*
 * Display current mode on status row
 */
static void update_mode_status(state_t *st) {
  const char *insert_mode = "-- INSERT --";
  const char *normal_mode = "-- NORMAL --";

  const char *text;

  if (st->mode == INSERT_BACK || st->mode == INSERT_FRONT) {
    text = insert_mode;
  } else if (st->mode == NORMAL) {
    text = normal_mode;
  } else {
    return;
  }

  clear_row(st->buf->status_row);
  for (size_t i = 0; i < strlen(text); i ++) {
    add_char(st->buf->status_row, text[i]);
  }
}

/*
 * Cursor position can be computed from:
 * buf->current_row, buf->current_char, scr->top_row
 */
static void update_cursor_position(state_t *st) {
  size_t line_size = st->buf->current->line_size;
  size_t current_row = st->buf->current_row;
  size_t current_char = st->buf->current_char;
  size_t top_row = st->top_row;

  // cuz cx and cy are "computed properties" ;)
  st->cx = 0;
  st->cy = 0;

  st->cy = current_row - top_row;

  if (line_size == 0) {
    st->cx = 0;
  } else if (st->cx >= line_size) {
    st->cx = line_size - 1;
  } else {
    st->cx = current_char;
  }

  if (st->mode == INSERT_BACK && st->buf->current->line_size != 0) {
    st->cx ++;
  }

  if (st->mode == EX) {
    st->cy = st->scr->num_windows;
    st->cx = st->buf->status_row->line_size;
  } else {
    st->cx = st->cx + st->padding_front + 1;
  }
}

/*
 * Determine current rows to be displayed and update windows <-> rows links
 * Can be computed from:
 * st->cy, buf->current_row, scr->top_window, scr->num_windows
 *
 * We want to update the display when:
 * - scroll up/down (update_top_row() -> true)
 * - insert/delete row(s)
 * - insert at the bottom which triggers a "scroll"
 */
static void update_scr_windows(state_t *st) {
  // link status window and its buffer
  if (!st->scr->status_window->r) {
    st->scr->status_window->r = st->buf->status_row;
  }

  size_t current_row = st->buf->current_row;
  size_t top_row = st->top_row;
  size_t num_windows = st->scr->num_windows;

  window_t **windows = st->scr->windows;

  row_t *r = st->buf->current;

  for (size_t i = top_row; i <= current_row; i ++) {
    windows[current_row - i]->r = r;
    windows[current_row - i]->line_number = top_row + current_row - i + 1;
    r->is_dirty = true;
    r = r->prev;
  }

  r = st->buf->current->next;

  for (size_t i = current_row + 1; i < top_row + num_windows; i ++) {
    if (r) {
      windows[i - top_row]->r = r;
      windows[i - top_row]->line_number = i + 1;
      r->is_dirty = true;
      r = r->next;
    } else {
      windows[i - top_row]->r = NULL;
      windows[i - top_row]->line_number = 0;
    }
  }
}

void update_state(state_t *st) {
  update_mode_status(st);
  update_top_row(st);
  update_padding_front(st);

  if (st->to_refresh) {
    update_scr_windows(st);
    st->to_refresh = false;
  }

  update_cursor_position(st);
}
