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
      handle_move(st, DOWN);
      break;
    case 'k':
      handle_move(st, UP);
      break;
    case 'h':
      handle_move(st, LEFT);
      break;
    case 'l':
      handle_move(st, RIGHT);
      break;
    case '$':
      handle_move_to_edge(st, RIGHT);
      break;
    case '0':
      handle_move_to_edge(st, LEFT);
      break;
    case 'G':
      handle_move_to_edge(st, DOWN);
      break;
    case 'g':
      if (st->prev_key == 'g') {
        handle_move_to_edge(st, UP);
        reset_prev_key(st);
      } else {
        set_prev_key(st, (char) ch);
      }
      break;
    case 'x':
      handle_delete_char(st);
      break;
    case 'd':
      if (st->prev_key == 'd') {
        handle_delete_row(st);
        reset_prev_key(st);
      } else {
        set_prev_key(st, (char) ch);
      }
      break;
    case 'I':
      handle_move_to_edge(st, LEFT);
    case 'i':
      if (st->buf->current->line_size == 0) {
        st->mode = INSERT_BACK;
      } else {
        st->mode = INSERT_FRONT;
      }
      break;
    case 'A':
      handle_move_to_edge(st, RIGHT);
    case 'a':
      st->mode = INSERT_BACK;
      break;
    case 'o':
      handle_append_row(st);
      break;
    case 'O':
      handle_prepend_row(st);
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
        handle_move(st, LEFT);
      }

      st->mode = NORMAL;
      break;
    default:
      handle_insert_char(st, (char) ch);
      break;
  }
}
