#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../src/state.h"

int test_update_cursor(const char *filename) {
  // cursor moved to an empty line
  state_t *st = init_state(filename);
  assert(st->cx == 0 && st->cy == 0);

  update_cursor_position(st);
  assert(st->cx == 0 && st->cy == 0);

  destroy_state(st);
}

int main(void) {
  const char *filename = "./file.txt";
  test_update_cursor(filename);

  return 0;
}
