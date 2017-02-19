#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "state.h"
#include "editor.h"
#include "screen.h"
#include "ex.h"

#define CMP1(s, s1) (strcmp(s, s1) == 0)
#define CMP2(s, s1, s2) (strcmp(s, s1) == 0 || strcmp(s, s2) == 0)

// linked list to array of chars
static char *to_string(echar_t *head, size_t length) {
  char *result = malloc((length + 1) * sizeof(char));

  echar_t *ec = head;
  for (size_t i = 0; ec && i < length; i++) {
    snprintf(result + i, length - i + 1, "%c", ec->c);
    ec = ec->next;
  }

  return result;
}

static char *row_to_string(void) {
  size_t buffer_size = g_state->num_rows;
  row_t *r = g_state->head;

  for (size_t i = 0; r && i < g_state->num_rows; i ++) {
    buffer_size += r->line_size;
    r = r->next;
  }

  char *result = malloc(buffer_size * sizeof(char));
  r = g_state->head;

  strcpy(result, "");
  while (r) {
    char *line = to_string(r->head->next, r->line_size);
    strcat(result, line);
    strcat(result, "\n");
    free(line);
    r = r->next;
  }

  return result;
}

static void quit(void) {
  destroy_editor();
  destroy_screen();
  exit(0);
}

static void save(void) {
  char *buffer = row_to_string();

  FILE *fp = fopen(g_state->filename, "w");
  fputs(buffer, fp);
  free(buffer);
  fclose(fp);
}

void ex_match_action(row_t *status) {
  // first char is the null char, second char is ':'
  char *command = to_string(status->head->next->next, status->line_size);

  if (CMP2(command, "q", "quit")) {
    free(command);
    quit();
  } else if (CMP1(command, "w")) {
    free(command);
    save();
  } else if (CMP1(command, "wq")) {
    free(command);
    save();
    quit();
  }
}
