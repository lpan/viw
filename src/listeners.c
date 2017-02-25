#include <ncurses.h>
#include "buffer.h"
#include "screen.h"
#include "render.h"
#include "state.h"
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
        start_insert_listener(st, prepend_char);
        break;
      case INSERT_BACK:
        start_insert_listener(st, append_char);
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
      break;
    case 'g':
      if (st->prev_key == 'g') {
        to_top(st->buf);
      }

      break;
    case 'x':
      delete_char(st->buf);
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

  st->prev_key = (char) ch;
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

void start_insert_listener(state_t *st, void (*insert)(buffer_t *, char)) {
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
      insert(st->buf, (char) ch);
      break;
  }
}
