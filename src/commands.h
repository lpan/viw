#ifndef COMMANDS_H
#define COMMANDS_H

#include "buffer.h"

typedef enum COMMAND_TYPE {
  HANDLE_MOVE,
  HANDLE_MOVE_TO_EDGE,
  HANDLE_INSERT_CHAR,
  HANDLE_APPEND_ROW,
  HANDLE_PREPEND_ROW,
  HANDLE_DELETE_CHAR,
  HANDLE_DELETE_ROW,
  HANDLE_ENTER,
  HANDLE_BACKSPACE
} COMMAND_TYPE;

typedef union COMMAND_PAYLOAD {
  char c;
  char* line;
  DIRECTION d;
} COMMAND_PAYLOAD;

typedef struct command {
  COMMAND_TYPE type;
  COMMAND_PAYLOAD payload;
  struct command *next;
  struct command *prev;
} command_t;

typedef struct history_stack {
  struct command *bottom;
  struct command *top;
} history_stack_t;

typedef struct future_queue {
  struct command *front;
  struct command *back;
} future_queue_t;

future_queue_t *init_future_queue(void);

history_stack_t *init_history_stack(void);

void destroy_future_queue(future_queue_t *fq);

void destroy_history_stack(history_stack_t *hs);

command_t *init_command(COMMAND_TYPE t, COMMAND_PAYLOAD p);

command_t *append_command(history_stack_t *hs, command_t *c);

command_t *pop_command(history_stack_t *hs);

void queue_command(future_queue_t *fq, command_t *c);

#endif
