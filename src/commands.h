#ifndef COMMANDS_H
#define COMMANDS_H

#include "state.h"

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

typedef struct command_queue {
  struct command *head;
  struct command *last;
  struct command *current;
} command_queue_t;

command_queue_t *init_command_queue(void);

void destroy_command_queue(command_queue_t *cq);

void append_command(command_queue_t *cq, COMMAND_TYPE t, COMMAND_PAYLOAD p);

void dispatch_command(command_t *c, state_t *st);

void replay_til_current(command_queue_t *cq, state_t *st);

#endif
