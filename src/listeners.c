#include <ncurses.h>
#include "screen.h"
#include "render.h"
#include "state.h"
#include "ex.h"
#include "listeners.h"

// we want to preserve cursor position when exit from ex mode
static void enter_ex(void) {
  clear_row(g_state->status);
  g_state->t_cx = g_state->cx;
  g_state->t_cy = g_state->r_cy;
  g_state->cx = 0;
  g_state->r_cy = g_screen->num_windows;
}

static void exit_ex(void) {
  g_state->cx = g_state->t_cx;
  g_state->r_cy = g_state->t_cy;
  g_state->t_cx = 0;
  g_state->t_cy = 0;
}

// enter insert mode with "i"
static void i_insert(void) {
  row_t *cur_row = g_state->current;
  cur_row->current = cur_row->current->prev;
}

static void ui_insert(void) {
  row_t *cur_row = g_state->current;
  g_state->cx = 0;
  cur_row->current = cur_row->head->next;
}

static void a_insert(void) {
  g_state->cx ++;
}

static void ua_insert(void) {
  row_t *cur_row = g_state->current;
  g_state->cx = cur_row->line_size;
  cur_row->current = cur_row->last;
}

static void o_insert(void) {
  append_row(NULL);

  g_state->cx = 0;
  g_state->cy ++;
  g_state->r_cy ++;

  row_t *r = g_state->current;

  for (size_t i = g_state->r_cy; r && i < g_screen->num_windows; i ++) {
    window_t *w = g_screen->windows[i];
    w->r = r;
    r->win = w;
    r = r->next;
  }

  render_some(next_row, g_screen->num_windows - g_state->r_cy);
}

static void uo_insert(void) {
  prepend_row(NULL);

  g_state->cx = 0;

  row_t *r = g_state->current;

  for (size_t i = g_state->r_cy; r && i < g_screen->num_windows; i ++) {
    window_t *w = g_screen->windows[i];
    w->r = r;
    r->win = w;
    r = r->next;
  }

  render_some(next_row, g_screen->num_windows - g_state->r_cy);
}

// enter insert mode with "a"
static void enter_insert(void (*f) (void)) {
  row_t *cur_row = g_state->current;

  // handle an empty row
  if (cur_row->current->c != '\0') {
    f();
  }
}

static void exit_insert(void) {
  g_state->cx --;

  // current node cant be the null char under 'normal' mode
  row_t *cur_row = g_state->current;
  if (cur_row->current->c == '\0') {
    g_state->cx ++;
    if (cur_row->current->next) {
      cur_row->current = cur_row->current->next;
    }
  }
}

void start_listener(void) {
  if (g_state->mode == NORMAL) {
    start_normal_listener();
  } else if (g_state->mode == EX) {
    start_ex_listener();
  } else if (g_state->mode == INSERT) {
    start_insert_listener();
  }
}

void start_normal_listener(void) {
  int ch = getch();
  switch (ch) {
    // navigations
    case 'j':
      down_row();
      break;
    case 'k':
      up_row();
      break;
    case 'h':
      left_column(g_state->current);
      break;
    case 'l':
      right_column(g_state->current);
      break;
    case 'x':
      delete_char(g_state->current);
      render_window(NULL, g_state->current);
      break;
    case ':':
      g_state->mode = EX;
      enter_ex();
      add_char(g_state->status, ':');
      render_window(g_screen->status_window, g_state->status);
      break;
    case 'i':
      g_state->mode = INSERT;
      enter_insert(i_insert);
      break;
    case 'I':
      g_state->mode = INSERT;
      enter_insert(ui_insert);
      break;
    case 'a':
      g_state->mode = INSERT;
      enter_insert(a_insert);
      break;
    case 'A':
      g_state->mode = INSERT;
      enter_insert(ua_insert);
      break;
    case 'o':
      g_state->mode = INSERT;
      enter_insert(o_insert);
      break;
    case 'O':
      g_state->mode = INSERT;
      enter_insert(uo_insert);
      break;
    default:
      break;
  }

  rerender();
  start_listener();
}

void start_ex_listener(void) {
  int ch = getch();
  switch (ch) {
    case '\n':
      ex_match_action(g_state->status);
      g_state->mode = NORMAL;
      exit_ex();
      break;
    case KEY_BACKSPACE:
      delete_char(g_state->status);
      break;
    default:
      add_char(g_state->status, (char) ch);
      break;
  }

  // exit EX mode when status bar is empty
  if (g_state->status->line_size == 0) {
    g_state->mode = NORMAL;
    exit_ex();
  }

  render_window(g_screen->status_window, g_state->status);
  rerender();
  start_listener();
}

void start_insert_listener(void) {
  int ch = getch();
  switch (ch) {
    case '\n':
      break;
    case KEY_BACKSPACE:
      backspace_char(g_state->current);
      render_window(NULL, g_state->current);
      break;
    case KEY_ESC:
      g_state->mode = NORMAL;
      exit_insert();
      break;
    default:
      add_char(g_state->current, (char) ch);
      render_window(NULL, g_state->current);
      break;
  }

  rerender();
  start_listener();
}

void start_visual_listener(void) {
}
