#include <ncurses.h>
#include <stdlib.h>
#include "screen.h"

static window_t **init_windows(size_t term_height) {
  // determine number of windows we want to display from terminal height
  size_t line_number = term_height - 1;
  const size_t height = 1, width = COLS;
  window_t **windows = malloc(line_number * sizeof(window_t *));

  for (size_t i = 0; i < line_number; i ++) {
    windows[i] = malloc(sizeof(window_t));
    windows[i]->w = newwin(height, width, i, 0);
    windows[i]->r = NULL;
  }

  return windows;
}

static window_t *init_status_window(size_t term_height) {
  const size_t height = 1, width = COLS;

  window_t *status_window = malloc(sizeof(window_t));
  status_window->w = newwin(height, width, term_height - 1, 0);
  status_window->r = NULL;

  return status_window;
}

static void destroy_window(window_t *w) {
  delwin(w->w);
  free(w);
}

static void destroy_windows(window_t **windows) {
  size_t line_number = LINES - 1;
  for (size_t i = 0; i < line_number; i ++) {
    destroy_window(windows[i]);
  }

  free(windows);
}

screen_t *init_screen(size_t term_height) {
  screen_t *scr = malloc(sizeof(screen_t));
  scr->num_windows = term_height - 1;
  scr->top_window = 0;
  scr->windows = init_windows(term_height);
  scr->status_window = init_status_window(term_height);

  return scr;
}

void destroy_screen(screen_t *scr) {
  destroy_windows(scr->windows);
  destroy_window(scr->status_window);
  free(scr);
}
