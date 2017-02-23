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

  st->buf = init_buffer(filename);
  st->scr = init_screen(LINES);

  update_cursor_position(st);

  return st;
}

void destroy_state(state_t *st) {
  destroy_buffer(st->buf);
  destroy_screen(st->scr);
  free(st);
}

void update_cursor_position(state_t *st) {
  size_t line_size = st->buf->current->line_size;
  size_t current_row = st->buf->current_row;
  size_t current_char = st->buf->current_char;
  size_t top_row = st->scr->top_row;

  st->cy = current_row - top_row;

  if (line_size == 0) {
    st->cx = 0;
  } else if (st->cx >= line_size) {
    st->cx = line_size - 1;
  } else {
    st->cx = current_char;
  }
}

/*
 * We want to update the display when:
 * - scroll up/down
 * - insert/delete row(s)
 * - insert at the bottom which triggers a "scroll"
 */
void update_display(state_t *st) {
  size_t current_row = st->buf->current_row;
  size_t top_row = st->scr->top_row;
  size_t num_windows = st->scr->num_windows;

  window_t **windows = st->scr->windows;

  row_t *r = st->buf->current;

  for (size_t i = top_row; i < current_row; i ++) {
    windows[top_row - i]->r = r;
    r = r->prev;
  }

  r = st->buf->current;

  for (size_t i = current_row; i < num_windows; i ++) {
    if (r) {
      windows[i - top_row]->r = r;
      r = r->next;
    } else {
      windows[i - top_row]->r = NULL;
    }
  }
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

row_t *next_row(row_t *r) {
  if (r->next) {
    return r = r->next;
  }
  return r;
}

row_t *prev_row(row_t *r) {
  if (r->prev) {
    return r = r->prev;
  }
  return r;
}

 * Handle vertical scrolling
 * It's technically O(1) since g_screen->num_windows is a constant ;)
static void adjust_windows(void) {
  // scroll down
  if (st->cy >= g_screen->top_window + g_screen->num_windows) {
    row_t *r = st->current;
    g_screen->top_window ++;

    for (size_t i = 0; i < g_screen->num_windows; i ++) {
      window_t *w = g_screen->windows[g_screen->num_windows - i - 1];
      w->r = r;
      r->win = w;
      r = r->prev;
    }

    st->r_cy --;

    // render from bottom to top
    render_all(prev_row);
  }

  // scroll up
  if (st->cy < g_screen->top_window) {
    row_t *r = st->current;
    g_screen->top_window --;

    for (size_t i = 0; i < g_screen->num_windows; i ++) {
      window_t *w = g_screen->windows[i];
      w->r = r;
      r->win = w;
      r = r->next;
    }

    st->r_cy ++;

    // render from top to bottom
    render_all(next_row);
  }
}
*/
