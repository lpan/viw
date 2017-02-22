#include <stdio.h>
#include <stdlib.h>
#include "buffer.h"
#include "screen.h"
#include "listeners.h"
#include "render.h"

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("Learn how to use an editor you noob!\n");
    exit(1);
  }

  char *filename = argv[1];

  // ncurses stuff
  initscr();
  raw();
  keypad(stdscr, TRUE);
  noecho();

  state_t *st = init_state(filename);

  initial_render(st);
  // start_listener(st);

  endwin();

  return 0;
}
