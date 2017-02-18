#include <stdio.h>
#include <stdlib.h>
#include "editor.h"
#include "window.h"
#include "listeners.h"

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("Learn how to use an editor you noob!\n");
    exit(1);
  }

  char *filename = argv[1];

  init_editor(filename);
  init_window();

  start_normal_listener();

  destroy_editor();
  destroy_window();

  return 0;
}
