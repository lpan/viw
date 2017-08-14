#ifndef HISTORY_H
#define HISTORY_H

#include <stdbool.h>
#include <stdlib.h>
#include "buffer.h"

typedef enum EVENT_TYPE {
  APPEND_CHAR,
  PREPEND_CHAR
} EVENT_TYPE;

typedef union EVENT_PAYLOAD {
  char c;
  char* line;
} EVENT_PAYLOAD;

typedef struct event {
  EVENT_TYPE type;
  EVENT_PAYLOAD payload;
  struct event *next;
  struct event *prev;
} event_t;

typedef struct event_queue {
  struct event *head;
  struct event *last;
} event_queue_t;

typedef struct history {
  struct event_queue *q;
  struct event *current;
} history_t;

void replay_til_current(buffer_t *b, history_t *h);

void undo_event(history_t *h);

void redo_event(history_t *h);

void append_event(history_t *h, event_t *e);

#endif
