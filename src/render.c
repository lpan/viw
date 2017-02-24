#include <ncurses.h>
#include "buffer.h"
#include "screen.h"
#include "render.h"

void render_window(window_t *w) {
  // only want to render dirty windows ;)
  if (!w->r->is_dirty) {
    return;
  }

  werase(w->w);

  echar_t *ch = w->r->head;
  while (ch) {
    wprintw(w->w, "%c", ch->c);
    ch = ch->next;
  }

  wprintw(w->w, "\n");
  wrefresh(w->w);
  w->r->is_dirty = false;
}

void render_windows(screen_t *scr) {
  render_window(scr->status_window);

  for (size_t i = 0; i < scr->num_windows; i ++) {
    window_t *w = scr->windows[i];

    // If row points to NULL, display empty row with ~
    if (!w->r) {
      werase(w->w);
      wprintw(w->w, "%c\n", '~');
      wrefresh(w->w);
    } else {
      render_window(w);
    }
  }
}

void render_update(state_t *st) {
  render_windows(st->scr);
  move(st->cy, st->cx);
  refresh();
}
