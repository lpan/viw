#include <ncurses.h>
#include "state.h"
#include "screen.h"
#include "render.h"

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

void initial_render(void) {
  // locate the top row to display
  row_t *r = g_state->head;
  for (size_t i = 0; i < g_screen->top_window; i ++) {
    r = r->next;
  }

  refresh();
  for (size_t i = 0; i < g_screen->num_windows; i ++) {
    render_window(g_screen->windows[i], r);
    if (r) {
      r->win = g_screen->windows[i];
      g_screen->windows[i]->r = r;
      r = r->next;
    }
  }

  // move cursor to initial position
  move(g_state->cy, g_state->cx);
}

void rerender(void) {
  move(g_state->cy, g_state->cx);
  refresh();
}
