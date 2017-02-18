#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "../src/state.h"
#include "../src/editor.h"

static void printBuffer(void) {
  row_t *r = g_state->head;
  while (r) {
    echar_t *c = r->head;
    while (c) {
      printf("%c", c->c);
      c = c->next;
    }
    printf("\n");
    r = r->next;
  }
}

static int mycmp(row_t *r, char *str) {
  const size_t s = 100;
  char buffer[s];
  echar_t *c = r->head;

  for (size_t i = 0; c; i++) {
    snprintf(buffer + i, s - i, "%c", c->c);
    c = c->next;
  }

  return strcmp(buffer, str);
}

int main(void) {
  const char *filename = "./tests/file.txt";

  init_editor(filename);
  assert(mycmp(g_state->current, "five") == 0);

  // move cursor up
  up_row();
  up_row();
  assert(mycmp(g_state->current, "three") == 0);

  // delete current row, cursor should be moved to next
  delete_row();
  assert(mycmp(g_state->current, "four") == 0);

  delete_row();
  assert(mycmp(g_state->current, "five") == 0);

  delete_row();
  assert(mycmp(g_state->current, "two") == 0);

  up_row();
  up_row();
  assert(mycmp(g_state->current, "zero") == 0);

  // test prepend at start
  prepend_row("goose");
  assert(mycmp(g_state->current, "goose") == 0);
  assert(mycmp(g_state->head, "goose") == 0);

  down_row();
  down_row();
  down_row();

  // test append at last
  append_row("paninos");
  assert(mycmp(g_state->current, "paninos") == 0);
  assert(mycmp(g_state->last, "paninos") == 0);

  destroy_editor();

  return 0;
}
