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
      // trim newline char
      line[strcspn(line, "\n")] = 0;
      append_row(line);
    }

    free(line);
    line = NULL;
    fclose(fp);
  }

  // insert empty row if file is empty or doesn't exist
  if (!g_state->current) {
    append_row(NULL);
  }

  // we want to start at the top when user opens up a new file
  g_state->current = g_state->head;
  g_state->current->current = g_state->current->head;
}

void destroy_editor(void) {
  destroy_state();
}
