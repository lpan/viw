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

int main(void) {
  const char *filename = "./tests/file.txt";

  init_editor(filename);
  assert(strcmp(g_state->current->buffer, "five\n") == 0);

  delete_row(5);
  assert(strcmp(g_state->current->buffer, "four\n") == 0);

  delete_row(0);
  assert(strcmp(g_state->current->buffer, "four\n") == 0);
  assert(strcmp(g_state->head->buffer, "one\n") == 0);

  insert_row(strdup("lmao"), 0);
  assert(strcmp(g_state->current->buffer, "lmao") == 0);
  assert(strcmp(g_state->head->buffer, "lmao") == 0);

  insert_row(strdup("goose"), 5);
  assert(strcmp(g_state->current->buffer, "goose") == 0);
  assert(strcmp(g_state->last->buffer, "goose") == 0);

  insert_row(strdup("paninos"), 3);
  assert(strcmp(g_state->current->buffer, "paninos") == 0);
  assert(g_state->current->index == 3);
  delete_row(3);
  assert(g_state->current->index == 2);

  destroy_editor();

  return 0;
}
