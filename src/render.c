#include <ncurses.h>
#include "state.h"
#include "screen.h"
#include "render.h"

/*
 * If row points to NULL, display empty row with ~
 * If win points to NULL, use r->win as window
 *
 * We only pass in win for initial render, most of the time, we want to call
 * this function like this `render_window(NULL, my_row)`
 */
void render_window(window_t *win, row_t *r) {
  if (r && r->win) {
    win = r->win;
  }

  // TODO diff algorithm
  werase(win->w);

  if (!r) {
    wprintw(win->w, "~");
  } else {
    // first char is always the null char
    echar_t *ch = r->head->next;
    while (ch) {
      wprintw(win->w, "%c", ch->c);
      ch = ch->next;
    }
  }

  wprintw(win->w, "\n");
  wrefresh(win->w);
}

void render_some(row_t *(*proceed) (row_t *), size_t n) {
  row_t *r = g_state->current;
  for (size_t i = 0; r && i < n; i ++) {
    render_window(NULL, r);
    r = proceed(r);
  }
}

/*
 * render top to bottom: r = r->next
 * render bottom to top: r = r->prev
 */
void render_all(row_t *(*proceed) (row_t *)) {
  render_some(proceed, g_screen->num_windows);
}

/*
 * first (LINES - 1) lines to the display
 */
void initial_render(void) {
  // locate the top row
  row_t *r = g_state->head;
  for (size_t i = 0; i < g_screen->top_window; i ++) {
    r = r->next;
  }

  refresh();
  for (size_t i = 0; i < g_screen->num_windows; i ++) {
    render_window(g_screen->windows[i], r);
    if (r) {
      // match first (LINES - 1) rows to windows
      r->win = g_screen->windows[i];
      g_screen->windows[i]->r = r;
      r = r->next;
    }
  }

  // move cursor to initial position
  move(g_state->r_cy, g_state->cx);
}

/*
 * readjust cursor position and refresh main window
 */
void rerender(void) {
  move(g_state->r_cy, g_state->cx);
  refresh();
}
