#include <ncurses.h>
#include "buffer.h"
#include "screen.h"
#include "render.h"
#include "state.h"
#include "controller.h"
#include "ex.h"
#include "listeners.h"

void start_listener(state_t *st) {
  while (true) {
    update_state(st);
    render_update(st);

    switch (st->mode) {
      case NORMAL:
        start_normal_listener(st);
        break;
      case INSERT_FRONT:
      case INSERT_BACK:
        start_insert_listener(st);
        break;
      case EX:
        start_ex_listener(st);
        break;
      case VISUAL:
        break;
    }
  }
}

void start_normal_listener(state_t *st) {
  int ch = getch();
  switch (ch) {
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
    case '$':
      to_right(st->buf);
      break;
    case '0':
      to_left(st->buf);
      break;
    case 'G':
      to_bottom(st->buf);
      st->to_refresh = true;
      break;
    case 'g':
      if (st->prev_key == 'g') {
        to_top(st->buf);
        st->to_refresh = true;
        reset_prev_key(st);
      } else {
        set_prev_key(st, (char) ch);
      }
      break;
    case 'x':
      delete_char(st->buf);
      break;
    case 'd':
      if (st->prev_key == 'd') {
        delete_row(st->buf);
        st->to_refresh = true;
        reset_prev_key(st);
      } else {
        set_prev_key(st, (char) ch);
      }
      break;
    case 'I':
      to_left(st->buf);
    case 'i':
      if (st->buf->current->line_size == 0) {
        st->mode = INSERT_BACK;
      } else {
        st->mode = INSERT_FRONT;
      }
      break;
    case 'A':
      to_right(st->buf);
    case 'a':
      st->mode = INSERT_BACK;
      break;
    case 'o':
      append_row(st->buf, NULL);
      st->to_refresh = true;
      st->mode = INSERT_BACK;
      break;
    case 'O':
      prepend_row(st->buf, NULL);
      st->to_refresh = true;
      st->mode = INSERT_BACK;
      break;
    case ':':
      st->mode = EX;
      clear_row(st->buf->status_row);
      add_char(st->buf->status_row, ':');
      break;
    default:
      break;
  }
}

void start_ex_listener(state_t *st) {
  int ch = getch();
  switch (ch) {
    case '\n':
      ex_match_action(st);
      st->mode = NORMAL;
      break;
    case KEY_BACKSPACE:
      handle_backspace(st);
      break;
    default:
      add_char(st->buf->status_row, (char) ch);
      break;
  }

  // exit EX mode when status bar is empty
  if (st->buf->status_row->line_size == 0) {
    st->mode = NORMAL;
  }
}

void start_insert_listener(state_t *st) {
  int ch = getch();
  switch (ch) {
    case '\n':
      handle_enter(st);
      break;
    case KEY_BACKSPACE:
      handle_backspace(st);
      break;
    case KEY_ESC:
      if (st->mode == INSERT_FRONT) {
        move_current(st->buf, LEFT);
      }

      st->mode = NORMAL;
      break;
    default:
      handle_insert(st, (char) ch);
      break;
  }
}
