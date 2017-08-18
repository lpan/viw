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

    switch (st->buf->mode) {
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
  COMMAND_PAYLOAD p;

  switch (ch) {
    case 'j':
      p.d = DOWN;
      apply_command(st, HANDLE_MOVE, p);
      break;
    case 'k':
      p.d = UP;
      apply_command(st, HANDLE_MOVE, p);
      break;
    case 'h':
      p.d = LEFT;
      apply_command(st, HANDLE_MOVE, p);
      break;
    case 'l':
      p.d = RIGHT;
      apply_command(st, HANDLE_MOVE, p);
      break;
    case '$':
      p.d = RIGHT;
      apply_command(st, HANDLE_MOVE_TO_EDGE, p);
      break;
    case '0':
      p.d = LEFT;
      apply_command(st, HANDLE_MOVE_TO_EDGE, p);
      break;
    case 'G':
      p.d = DOWN;
      apply_command(st, HANDLE_MOVE_TO_EDGE, p);
      break;
    case 'g':
      if (st->prev_key == 'g') {
        p.d = UP;
        apply_command(st, HANDLE_MOVE_TO_EDGE, p);
        reset_prev_key(st);
      } else {
        set_prev_key(st, (char) ch);
      }
      break;
    case 'x':
      apply_command(st, HANDLE_DELETE_CHAR, p);
      break;
    case 'd':
      if (st->prev_key == 'd') {
        apply_command(st, HANDLE_DELETE_ROW, p);
        reset_prev_key(st);
      } else {
        set_prev_key(st, (char) ch);
      }
      break;
    case 'I':
      p.d = LEFT;
      apply_command(st, HANDLE_MOVE_TO_EDGE, p);
    case 'i':
      if (st->buf->current->line_size == 0) {
        st->buf->mode = INSERT_BACK;
      } else {
        st->buf->mode = INSERT_FRONT;
      }
      break;
    case 'A':
      p.d = RIGHT;
      apply_command(st, HANDLE_MOVE_TO_EDGE, p);
    case 'a':
      st->buf->mode = INSERT_BACK;
      break;
    case 'o':
      apply_command(st, HANDLE_APPEND_ROW, p);
      break;
    case 'O':
      apply_command(st, HANDLE_PREPEND_ROW, p);
      break;
    case 'u':
      undo_command(st);
      break;
    case ':':
      st->buf->mode = EX;
      clear_row(st->status_row);
      add_char(st->status_row, ':');
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
      st->buf->mode = NORMAL;
      break;
    case KEY_BACKSPACE:
      handle_backspace(st);
      break;
    case KEY_ESC:
      st->buf->mode = NORMAL;
      break;
    default:
      add_char(st->status_row, (char) ch);
      break;
  }

  // exit EX mode when status bar is empty
  if (st->status_row->line_size == 0) {
    st->buf->mode = NORMAL;
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
      if (st->buf->mode == INSERT_FRONT) {
        handle_move(st, LEFT);
      }

      st->buf->mode = NORMAL;
      break;
    default:
      handle_insert_char(st, (char) ch);
      break;
  }
}
