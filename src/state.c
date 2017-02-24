#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "screen.h"
#include "buffer.h"
#include "state.h"

state_t *init_state(const char *filename) {
  state_t *st = malloc(sizeof(state_t));

  st->mode = NORMAL;
  st->cx = 0;
  st->cy = 0;
  st->top_row = 0;

  st->buf = init_buffer(filename);
  st->scr = init_screen(LINES);

  update_cursor_position(st);
  update_scr_windows(st);

  return st;
}

void destroy_state(state_t *st) {
  destroy_buffer(st->buf);
  destroy_screen(st->scr);
  free(st);
}

int update_top_row(state_t *st) {
  size_t current_row = st->buf->current_row;
  size_t num_windows = st->scr->num_windows;

  // scroll down
  if (current_row >= st->top_row + num_windows) {
    st->top_row = current_row - num_windows + 1;
    return 1;
  }

  // scroll up
  if (current_row < st->top_row) {
    st->top_row = current_row;
    return 1;
  }

  return 0;
}

void update_cursor_position(state_t *st) {
  size_t line_size = st->buf->current->line_size;
  size_t current_row = st->buf->current_row;
  size_t current_char = st->buf->current_char;
  size_t top_row = st->top_row;

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
}

/*
 * We want to update the display when:
 * - scroll up/down (update_top_row() -> true)
 * - insert/delete row(s)
 * - insert at the bottom which triggers a "scroll"
 */
void update_scr_windows(state_t *st) {
  size_t current_row = st->buf->current_row;
  size_t top_row = st->top_row;
  size_t num_windows = st->scr->num_windows;

  window_t **windows = st->scr->windows;

  row_t *r = st->buf->current;

  for (size_t i = top_row; i <= current_row; i ++) {
    windows[current_row - i]->r = r;
    r->is_dirty = true;
    r = r->prev;
  }

  r = st->buf->current;

  for (size_t i = current_row; i < top_row + num_windows; i ++) {
    if (r) {
      windows[i - top_row]->r = r;
      r->is_dirty = true;
      r = r->next;
    } else {
      windows[i - top_row]->r = NULL;
    }
  }
}

void move_cursor(state_t *st, DIRECTION d) {
  move_current(st->buf, d);

  bool scrolled = update_top_row(st);

  if (scrolled) {
    update_scr_windows(st);
  }
}

void insert_char(state_t *st, char c) {
  append_char(st->buf, c);
  update_cursor_position(st);
}

/*
 * delete_char() insert/ex mode version
 *
 * Always adjust cursor position
 *
 * Delete the char before 'current'
 * Delete line when current is the NULL char and append the rest of the line to
 * the previous line
void backspace_char(row_t *r) {
  if (r->current->c == '\0') {
    // delete line
    return;
  }

  delete_char(r);

  // if current is set to the NULL char
  // it means we now have an empty row
  if (r->current->c == '\0') {
    st->cx --;
    return;
  }

  // delete char will not adjust cursor if next char is not NULL
  if (r->current != r->last) {
    st->cx --;
    r->current = r->current->prev;
  }
}
*/
