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
}

/*
void add_char(row_t *r, char c) {
  append_char(r, c);
  st->cx ++;
}

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

 * Handle situations such as cx > row->line_size
 */
/*
static void adjust_x_cursor(void) {
  size_t cx = 0;
  row_t *cur_row = st->current;

  // handle empty row case
  if (!cur_row->head->next) {
    st->cx = cx;
    return;
  }

  cur_row->current = cur_row->head->next;

  for (size_t i = 0;
      i < st->cx &&
      cur_row->current &&
      cur_row->current->next;
      i ++) {
    cur_row->current = cur_row->current->next;
    cx ++;
  }
  st->cx = cx;
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
