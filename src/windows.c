#include <ncurses.h>
#include <stdlib.h>
#include "windows.h"

window_t **g_windows;
window_t *g_status_window;

static window_t **init_windows(void) {
  // determine number of windows we want to display from terminal height
  // TODO adjust this number when the height changes
  size_t line_number = LINES - 1;
  const size_t height = 1, width = COLS;
  window_t **windows = malloc(line_number * sizeof(window_t *));

  for (size_t i = 0; i < line_number; i ++) {
    windows[i] = malloc(sizeof(window_t));
    windows[i]->w = newwin(height, width, 0, i);
    windows[i]->r = NULL;
  }

  return windows;
}

static window_t *init_status_window(void) {
  const size_t height = 1, width = COLS;

  window_t *status_window = malloc(sizeof(window_t));
  status_window->w = newwin(height, width, 0, LINES - 1);
  status_window->r = NULL;

  return status_window;
}

static void destroy_window(window_t *w) {
  delwin(w->w);
  free(w);
}

static void destroy_windows(void) {
  size_t line_number = LINES - 1;
  for (size_t i = 0; i < line_number; i ++) {
    destroy_window(g_windows[i]);
  }

  destroy_window(g_status_window);
  free(g_windows);
}

void init_screen(void) {
  initscr();
  // raw();
  keypad(stdscr, TRUE);
  noecho();

  g_windows = init_windows();
  g_status_window = init_status_window();
}

void destroy_screen(void) {
  destroy_windows();
  endwin();
}
