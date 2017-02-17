#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "editor.h"
#include "state.h"

void init_editor(const char *filename) {
  // init global state
  init_state(filename);

  // read file to memory
  FILE *fp = fopen(filename, "r");
  if (fp) {
    char *line = NULL;
    size_t len = 0;

    for (size_t i = 0; getline(&line, &len, fp) != -1; i++) {
      insert_row(strdup(line), i);
    }

    free(line);
    line = NULL;
    fclose(fp);
  }

  // insert empty row if file is empty or doesn't exist
  if (!g_state->current) {
    insert_row(NULL, 0);
  }
}

void destroy_editor(void) {
  destroy_state();
}
