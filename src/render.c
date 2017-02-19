#include <ncurses.h>
#include "state.h"
#include "screen.h"

static void render_window(WINDOW *w, row_t *r) {
  if (!r) {
    wprintw(w, "~");
  } else {
    echar_t *ch = r->head;
    while (ch) {
      wprintw(w, "%c", ch->c);
      ch = ch->next;
    }
  }

  wprintw(w, "\n");
  wrefresh(w);
}

void initial_render(void) {
  // locate the top row to display
  row_t *top = g_state->head;
  for (size_t i = 0; i < g_screen->top_window; i ++) {
    top = top->next;
  }

  refresh();
  for (size_t i = 0; i < g_screen->num_windows; i ++) {
    render_window(g_screen->windows[i]->w, top);
    if (top) {
      top = top->next;
    }
  }
}
