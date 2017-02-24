#include <ncurses.h>
#include "buffer.h"
#include "screen.h"
#include "render.h"
#include "state.h"
#include "ex.h"
#include "listeners.h"

void start_listener(state_t *st) {
  while (true) {
    update_cursor_position(st);
    render_update(st);
    if (st->mode == NORMAL) {
      start_normal_listener(st);
    } else if (st->mode == INSERT_FRONT) {
      start_insert_listener(st, prepend_char);
    } else if (st->mode == INSERT_BACK) {
      start_insert_listener(st, append_char);
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
    case 'x':
      delete_char(st->buf);
      break;
    case '$':
      to_right(st->buf);
      break;
    case '0':
      to_left(st->buf);
      break;
    /*
    case ':':
      st->mode = EX;
      enter_ex();
      add_char(st->status, ':');
      render_window(g_screen->status_window, st->status);
      break;
      */
    case 'i':
      if (st->buf->current->line_size == 0) {
        st->mode = INSERT_BACK;
      } else {
        st->mode = INSERT_FRONT;
      }
      break;
    case 'a':
      st->mode = INSERT_BACK;
      break;
    case 'A':
      st->mode = INSERT_BACK;
      to_right(st->buf);
      break;
    case 'I':
      if (st->buf->current->line_size == 0) {
        st->mode = INSERT_BACK;
      } else {
        st->mode = INSERT_FRONT;
      }
      to_left(st->buf);
      break;
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
*/

void start_insert_listener(state_t *st, void (*insert)(buffer_t *, char)) {
  int ch = getch();
  switch (ch) {
    case '\n':
      break;
    case KEY_BACKSPACE:
      break;
    case KEY_ESC:
      if (st->mode == INSERT_FRONT) {
        move_current(st->buf, LEFT);
      }

      st->mode = NORMAL;
      break;
    default:
      insert(st->buf, (char) ch);
      break;
  }
}
