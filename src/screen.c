#include <ncurses.h>
#include <stdlib.h>
#include "screen.h"

screen_t *g_screen;

static window_t **init_windows(void) {
  // determine number of windows we want to display from terminal height
  // TODO adjust this number when the height changes
  size_t line_number = g_screen->num_windows;
  const size_t height = 1, width = COLS;
  window_t **windows = malloc(line_number * sizeof(window_t *));

  for (size_t i = 0; i < line_number; i ++) {
    windows[i] = malloc(sizeof(window_t));
    windows[i]->w = newwin(height, width, i, 0);
    windows[i]->r = NULL;
  }

  return windows;
}

static window_t *init_status_window(void) {
  const size_t height = 1, width = COLS;

  window_t *status_window = malloc(sizeof(window_t));
  status_window->w = newwin(height, width, 0, g_screen->num_windows);
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

void init_screen(void) {
  initscr();
  // raw();
  keypad(stdscr, TRUE);
  noecho();

  g_screen = malloc(sizeof(screen_t));
  g_screen->num_windows = LINES - 1;
  g_screen->top_window = 0;
  g_screen->windows = init_windows();
  g_screen->status_window = init_status_window();
}

void destroy_screen(void) {
  destroy_windows(g_screen->windows);
  destroy_window(g_screen->status_window);
  free(g_screen);
  endwin();
}
