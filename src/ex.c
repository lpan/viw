#include <stdlib.h>
#include <string.h>
#include "state.h"
#include "editor.h"
#include "screen.h"
#include "ex.h"

#define CMP1(s, s1) (strcmp(s, s1) == 0)
#define CMP2(s, s1, s2) (strcmp(s, s1) == 0 || strcmp(s, s2) == 0)

// linked list to array of chars
static char* to_string(row_t *r) {
  char *result = malloc(r->line_size * sizeof(char));
  echar_t *ec = r->head->next;
  for (size_t i = 0; ec && i < r->line_size; i++) {
    snprintf(result + i, r->line_size - i, "%c", ec->c);
    ec = ec->next;
  }

  return result;
}

void ex_match_action(row_t *status) {
  char *command = to_string(status);

  if (CMP2(command, "q", "quit")) {
    free(command);
    destroy_editor();
    destroy_screen();
    exit(0);
  }
}
