#include <ncurses.h>
#include "buffer.h"
#include "screen.h"
#include "render.h"
#include "state.h"
#include "ex.h"
#include "listeners.h"

/*
 * we want to preserve cursor position when exit from ex mode
static void enter_ex(state_t *st, buffer_t *buf) {
  clear_row(buf->status_row);
  st->t_cx = st->cx;
  st->t_cy = st->cy;
  st->cx = 0;
  st->cy = g_screen->num_windows;
}

static void exit_ex(state_t *st) {
  st->cx = st->t_cx;
  st->cy = st->t_cy;
  st->t_cx = 0;
  st->t_cy = 0;
}
*/

/*
// enter insert mode with "i"
static void i_insert(state_t *st, buffer_t *buf) {
  row_t *cur_row = buf->current;
  cur_row->current = cur_row->current->prev;
}

static void ui_insert(state_t *st, buffer_t *buf) {
  row_t *cur_row = st->current;
  st->cx = 0;
  cur_row->current = cur_row->head->next;
}

static void a_insert(state_t *st, buffer_t *buf) {
  st->cx ++;
}

static void ua_insert(state_t *st, buffer_t *buf) {
  row_t *cur_row = st->current;
  st->cx = cur_row->line_size;
  cur_row->current = cur_row->last;
}

static void o_insert(state_t *st, buffer_t *buf) {
  append_row(NULL);

  st->cx = 0;
  st->cy ++;
  st->r_cy ++;

  row_t *r = st->current;

  for (size_t i = st->r_cy; r && i < g_screen->num_windows; i ++) {
    window_t *w = g_screen->windows[i];
    w->r = r;
    r->win = w;
    r = r->next;
  }

  render_some(next_row, g_screen->num_windows - st->r_cy);
}

static void uo_insert(state_t *st, buffer_t *buf) {
  prepend_row(NULL);

  st->cx = 0;

  row_t *r = st->current;

  for (size_t i = st->r_cy; r && i < g_screen->num_windows; i ++) {
    window_t *w = g_screen->windows[i];
    w->r = r;
    r->win = w;
    r = r->next;
  }

  render_some(next_row, g_screen->num_windows - st->r_cy);
}

// enter insert mode with "a"
static void enter_insert(void (*f) (state_t *st, buffer_t *buf)) {
  row_t *cur_row = st->current;

  // handle an empty row
  if (cur_row->current->c != '\0') {
    f();
  }
}

static void exit_insert(state_t *st, buffer_t *buf) {
  st->cx --;

  // current node cant be the null char under 'normal' mode
  row_t *cur_row = st->current;
  if (cur_row->current->c == '\0') {
    st->cx ++;
    if (cur_row->current->next) {
      cur_row->current = cur_row->current->next;
    }
  }
}
*/

void start_listener(state_t *st) {
  while (true) {
    render_update(st);
    if (st->mode == NORMAL) {
      start_normal_listener(st);
      /*
    } else if (st->mode == EX) {
      start_ex_listener(st, buf);
    } else if (st->mode == INSERT) {
      start_insert_listener(st, buf);
      */
    }
  }
}

void start_normal_listener(state_t *st) {
  int ch = getch();
  switch (ch) {
    // navigations
    case 'j':
      move_cursor(st, DOWN);
      break;
    case 'k':
      move_cursor(st, UP);
      break;
    case 'h':
      move_cursor(st, LEFT);
      break;
    case 'l':
      move_cursor(st, RIGHT);
      break;
    /*
    case 'x':
      delete_char(st->current);
      render_window(NULL, st->current);
      break;
    case ':':
      st->mode = EX;
      enter_ex();
      add_char(st->status, ':');
      render_window(g_screen->status_window, st->status);
      break;
    case 'i':
      st->mode = INSERT;
      enter_insert(i_insert);
      break;
    case 'I':
      st->mode = INSERT;
      enter_insert(ui_insert);
      break;
    case 'a':
      st->mode = INSERT;
      enter_insert(a_insert);
      break;
    case 'A':
      st->mode = INSERT;
      enter_insert(ua_insert);
      break;
    case 'o':
      st->mode = INSERT;
      enter_insert(o_insert);
      break;
    case 'O':
      st->mode = INSERT;
      enter_insert(uo_insert);
      break;
    */
    default:
      break;
  }
}

/*
void start_ex_listener(state_t *st, buffer_t *buf) {
  int ch = getch();
  switch (ch) {
    case '\n':
      ex_match_action(st->status);
      st->mode = NORMAL;
      exit_ex();
      break;
    case KEY_BACKSPACE:
      delete_char(st->status);
      break;
    default:
      add_char(st->status, (char) ch);
      break;
  }

  // exit EX mode when status bar is empty
  if (st->status->line_size == 0) {
    st->mode = NORMAL;
    exit_ex();
  }

  render_window(g_screen->status_window, st->status);
}

void start_insert_listener(state_t *st, buffer_t *buf) {
  int ch = getch();
  switch (ch) {
    case '\n':
      break;
    case KEY_BACKSPACE:
      backspace_char(st->current);
      render_window(NULL, st->current);
      break;
    case KEY_ESC:
      st->mode = NORMAL;
      exit_insert();
      break;
    default:
      add_char(st->current, (char) ch);
      render_window(NULL, st->current);
      break;
  }
}
*/
