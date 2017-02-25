#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "state.h"
#include "screen.h"
#include "ex.h"

#define CMP1(s, s1) (strcmp(s, s1) == 0)
#define CMP2(s, s1, s2) (strcmp(s, s1) == 0 || strcmp(s, s2) == 0)

// linked list to array of chars
static char *to_string(echar_t *head, size_t length) {
  char *result = malloc((length + 1) * sizeof(char));
  result[0] = '\0';

  echar_t *ec = head;
  for (size_t i = 0; ec && i < length; i++) {
    snprintf(result + i, length - i + 1, "%c", ec->c);
    ec = ec->next;
  }

  return result;
}

static char *rows_to_string(buffer_t *buf) {
  size_t buffer_size = buf->num_rows;
  row_t *r = buf->head;

  for (size_t i = 0; r && i < buf->num_rows; i ++) {
    buffer_size += r->line_size;
    r = r->next;
  }

  char *result = malloc(buffer_size * sizeof(char));
  r = buf->head;

  strcpy(result, "");
  while (r) {
    char *line = to_string(r->head, r->line_size);
    strcat(result, line);

    // no newline for the last line
    if (r->next) {
      strcat(result, "\n");
    }
    free(line);
    r = r->next;
  }

  return result;
}

static void quit(state_t *st) {
  destroy_state(st);
  endwin();
  exit(0);
}

static void save(state_t *st) {
  char *buf = rows_to_string(st->buf);

  FILE *fp = fopen(st->buf->filename, "w");
  fputs(buf, fp);
  free(buf);
  fclose(fp);
}

void ex_match_action(state_t *st) {
  // first char is ':'
  char *command = to_string(st->buf->status_row->head->next, st->buf->status_row->line_size);

  if (CMP2(command, "q", "quit")) {
    free(command);
    quit(st);
  } else if (CMP1(command, "w")) {
    free(command);
    save(st);
  } else if (CMP1(command, "wq")) {
    free(command);
    save(st);
    quit(st);
  }
}
