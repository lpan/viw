#include <stdio.h>
#include <stdlib.h>
#include "editor.h"

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("Learn how to use an editor you noob!\n");
    exit(1);
  }

  char *filename = argv[1];

  init_editor(filename);
  destroy_editor();

  return 0;
}
