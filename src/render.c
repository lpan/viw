#include <ncurses.h>
#include "buffer.h"
#include "screen.h"
#include "render.h"

void render_window(window_t *w, size_t padding_front) {
  // only want to render dirty windows ;)
  if (!w->r->is_dirty) {
    return;
  }

  werase(w->w);

  // render line number
  if (w->line_number > 0) {
    size_t num_digits = 0;
    size_t line_number = w->line_number;

    while (line_number > 0) {
      line_number /= 10;
      num_digits ++;
    }

    wattron(w->w, A_BOLD);
    for (size_t i = 0; i < padding_front - num_digits; i ++) {
      wprintw(w->w, "%c", ' ');
    }
    wprintw(w->w, "%zu ", w->line_number);

    wattroff(w->w, A_BOLD);
  }

  echar_t *ch = w->r->head;
  while (ch) {
    wprintw(w->w, "%c", ch->c);
    ch = ch->next;
  }

  wprintw(w->w, "\n");
  wrefresh(w->w);
  w->r->is_dirty = false;
}

void render_windows(state_t *st) {
  screen_t *scr = st->scr;
  render_window(scr->status_window, 0);

  for (size_t i = 0; i < scr->num_windows; i ++) {
    window_t *w = scr->windows[i];

    // If row points to NULL, display empty row with ~
    if (!w->r) {
      werase(w->w);
      wprintw(w->w, "%c\n", '~');
      wrefresh(w->w);
    } else {
      render_window(w, st->padding_front);
    }
  }
}

void render_update(state_t *st) {
  render_windows(st);
  move(st->cy, st->cx);
  refresh();
}
