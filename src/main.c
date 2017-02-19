#include <stdio.h>
#include <stdlib.h>
#include "editor.h"
#include "windows.h"
#include "listeners.h"
#include "render.h"

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("Learn how to use an editor you noob!\n");
    exit(1);
  }

  char *filename = argv[1];

  init_screen();
  init_editor(filename);

  initial_render();

  start_normal_listener();

  destroy_editor();
  destroy_screen();

  return 0;
}
