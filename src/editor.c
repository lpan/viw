#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "editor.h"
#include "state.h"

void init_editor(char *filename) {
  // init global state
  init_state(filename);

  // read file to memory
  FILE *fp = fopen(filename, "r");
  if (fp) {
    char *line = NULL;
    size_t len = 0;

    while (getline(&line, &len, fp) != -1) {
      add_row(strdup(line), len);
      printf("%s", line);
    }

    free(line);
  } else {
    // add an empty row if file does not exist
    add_row(NULL, 0);
  }

  fclose(fp);
}

void destroy_editor(void) {
  destroy_state();
}
