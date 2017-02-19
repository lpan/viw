#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "state.h"

state_t *g_state;

static void destroy_row(row_t *r) {
  echar_t *c = r->head, *tmp;
  while (c) {
    tmp = c;
    c = c->next;
    free(tmp);
  }
  free(r);
}

void destroy_state(void) {
  row_t *r = g_state->head, *tmp;
  while (r) {
    tmp = r;
    r = r->next;
    destroy_row(tmp);
  }

  free(g_state);
  g_state = NULL;
}

static void append_char(row_t *r, char c) {
  echar_t *ec = malloc(sizeof(echar_t));
  echar_t *next = NULL;
  echar_t *prev = NULL;

  ec->c = c;
  ec->prev = NULL;
  ec->next = NULL;

  if (r->current == r->last) {
    prev = r->current;
    prev->next = ec;
    r->last = ec;
  } else {
    prev = r->current;
    next = r->current->next;
    prev->next = ec;
    next->prev = ec;
  }

  ec->next = next;
  ec->prev = prev;

  r->line_size ++;
  r->current = ec;
}

/*
 * There is always a NULL char at the beginning of every row
 */
static row_t *init_row(const char *buffer) {
  row_t *r = malloc(sizeof(row_t));

  // null char
  echar_t *ec = malloc(sizeof(echar_t));
  ec->c = '\0';
  ec->prev = NULL;
  ec->next = NULL;

  r->win = NULL;
  r->next = NULL;
  r->prev = NULL;
  r->line_size = 0;

  r->head = ec;
  r->last = ec;
  r->current = ec;

  if (!buffer) {
    return r;
  }

  for (size_t i = 0; i < strlen(buffer); i ++) {
    append_char(r, buffer[i]);
  }
  return r;
}

void init_state(const char *filename) {
  g_state = malloc(sizeof(state_t));

  g_state->cx = 0;
  g_state->cy = 0;

  g_state->t_cx = 0;
  g_state->t_cy = 0;

  g_state->is_dirty = false;
  g_state->filename = filename;

  g_state->num_rows = 0;
  g_state->status = init_row(NULL);
  g_state->head = NULL;
  g_state->last = NULL;
  g_state->current = NULL;

  g_state->mode = NORMAL;
}

void add_char(row_t *r, char c) {
  append_char(r, c);
  g_state->cx ++;
}

/*
 * delete current char and set 'current' to point to next char
 *
 * Adjust the cursor position only when next char is NULL
 *
 * if only NULL char is present, do nothing
 * if next char is NULL, point to the previous char
 */
void delete_char(row_t *r) {
  if (r->line_size == 0) {
    return;
  }

  echar_t *to_delete = r->current;

  if (to_delete == r->head) {
    r->current = to_delete->next;
    r->head = r->current;
    r->head->prev = NULL;
  } else if (to_delete == r->last) {
    r->current = to_delete->prev;
    r->last = r->current;
    r->last->next = NULL;

    // handle empty row
    if (r->current->c != '\0') {
      g_state->cx --;
    }
  } else {
    r->current = to_delete->next;
    r->current->prev = to_delete->prev;
    to_delete->prev->next = r->current;
  }

  r->line_size --;
  free(to_delete);
}

/*
 * delete_char() insert/ex mode version
 *
 * Always adjust cursor position
 *
 * Delete the char before 'current'
 * Delete line when current is the NULL char and append the rest of the line to
 * the previous line
 */
void backspace_char(row_t *r) {
  if (r->current->c == '\0') {
    // delete line
    return;
  }

  delete_char(r);

  // if current is set to the NULL char
  // it means we now have an empty row
  if (r->current->c == '\0') {
    g_state->cx --;
    return;
  }

  // delete char will not adjust cursor if next char is not NULL
  if (r->current != r->last) {
    g_state->cx --;
    r->current = r->current->prev;
  }
}

// insert row after current
void append_row(const char *buffer) {
  row_t *r = init_row(buffer);
  row_t *prev = NULL;
  row_t *next = NULL;

  if (g_state->num_rows == 0) {
    g_state->head = r;
    g_state->last= r;
  } else if (g_state->current == g_state->last) {
    prev = g_state->current;
    prev->next = r;
    g_state->last = r;
  } else {
    prev = g_state->current;
    next = g_state->current->next;
    prev->next = r;
    next->prev = r;
  }

  r->next = next;
  r->prev = prev;

  g_state->num_rows ++;
  g_state->current = r;
}

// insert row before current
void prepend_row(const char *buffer) {
  row_t *r = init_row(buffer);
  row_t *prev = NULL;
  row_t *next = NULL;

  if (g_state->num_rows == 0) {
    g_state->head = r;
    g_state->last= r;
  } else if (g_state->current == g_state->head) {
    next = g_state->current;
    next->prev = r;
    g_state->head = r;
  } else {
    prev = g_state->current->prev;
    next = g_state->current;
    prev->next = r;
    next->prev = r;
  }

  r->next = next;
  r->prev = prev;

  g_state->num_rows ++;
  g_state->current = r;
}

/*
 * Delete current row and reset 'current' to point to the next row.
 * If next is null then set it to previous
 * If there is only one row left, we simply "clear" the row
 */
void delete_row(void) {
  assert(g_state->num_rows > 0);
  row_t *to_delete = g_state->current;

  if (g_state->num_rows == 1) {
    clear_row(g_state->current);
    return;
  }

  if (to_delete == g_state->head) {
    g_state->current = to_delete->next;
    g_state->current->prev = NULL;
    g_state->head = g_state->current;
  } else if (to_delete == g_state->last) {
    g_state->current = to_delete->prev;
    g_state->current->next = NULL;
    g_state->last = g_state->current;
  } else {
    to_delete->prev->next = to_delete->next;
    to_delete->next->prev = to_delete->prev;
    g_state->current = to_delete->next;
  }

  g_state->num_rows --;
  destroy_row(to_delete);
}

void clear_row(row_t *r) {
  echar_t *ec = r->head->next;
  while (ec) {
    echar_t *tmp = ec;
    ec = ec->next;
    free(tmp);
  }

  r->win = NULL;
  r->last = r->head;
  r->current = r->head;
  r->line_size = 0;
}

/* Navigations */

/*
 * Handle situations such as cx > row->line_size
 */
static void adjust_x_cursor(void) {
  size_t cx = 0;
  row_t *cur_row = g_state->current;

  // handle empty row case
  if (!cur_row->head->next) {
    g_state->cx = cx;
    return;
  }

  cur_row->current = cur_row->head->next;

  for (size_t i = 0;
      i < g_state->cx &&
      cur_row->current &&
      cur_row->current->next;
      i ++) {
    cur_row->current = cur_row->current->next;
    cx ++;
  }
  g_state->cx = cx;
}

void up_row(void) {
  if (g_state->current->prev) {
    g_state->current = g_state->current->prev;
    g_state->cy --;

    adjust_x_cursor();
  }
}

void down_row(void) {
  if (g_state->current->next) {
    g_state->current = g_state->current->next;
    g_state->cy ++;

    adjust_x_cursor();
  }
}

void left_column(row_t *r) {
  if (r->current->prev && r->current->prev->c != '\0') {
    r->current = r->current->prev;
    g_state->cx --;
  }
}

void right_column(row_t *r) {
  if (r->current && r->current->next) {
    r->current = r->current->next;
    g_state->cx ++;
  }
}
