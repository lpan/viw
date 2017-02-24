#include <stdio.h>
#include <stdlib.h>
#include "listeners.h"
#include "state.h"

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("Learn how to use an editor you noob!\n");
    exit(1);
  }

  char *filename = argv[1];

  // elim escape key delay
  putenv("ESCDELAY=0");

  // ncurses stuff
  initscr();
  raw();
  keypad(stdscr, TRUE);
  noecho();
  refresh();

  state_t *st = init_state(filename);

  start_listener(st);

  endwin();
  return 0;
}
