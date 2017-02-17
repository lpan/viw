#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "../src/state.h"
#include "../src/editor.h"

static void printBuffer(void) {
  row_t *r = g_state->head;
  while (r) {
    printf("%s\n", r->buffer);
    r = r->next;
  }
}

static void printCurrent(void) {
  printf("%s\n", g_state->current->buffer);
}

int main(void) {
  const char *filename = "./tests/file.txt";

  init_editor(filename);
  assert(strcmp(g_state->current->buffer, "five") == 0);

  // move cursor up
  up_row();
  up_row();
  assert(strcmp(g_state->current->buffer, "three") == 0);

  // delete current row, cursor should be moved to next
  delete_row();
  assert(strcmp(g_state->current->buffer, "four") == 0);

  delete_row();
  assert(strcmp(g_state->current->buffer, "five") == 0);

  delete_row();
  assert(strcmp(g_state->current->buffer, "two") == 0);

  up_row();
  up_row();
  assert(strcmp(g_state->current->buffer, "zero") == 0);

  // test prepend at start
  prepend_row(strdup("goose"));
  assert(strcmp(g_state->current->buffer, "goose") == 0);
  assert(strcmp(g_state->head->buffer, "goose") == 0);

  down_row();
  down_row();
  down_row();

  // test append at last
  append_row(strdup("paninos"));
  assert(strcmp(g_state->current->buffer, "paninos") == 0);
  assert(strcmp(g_state->last->buffer, "paninos") == 0);

  destroy_editor();

  return 0;
}
