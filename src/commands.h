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

typedef struct command_stack {
  struct command *bottom;
  struct command *top;
} command_stack_t;

command_stack_t *init_command_stack(void);

void destroy_command_stack(command_stack_t *cs);

command_t *init_command(COMMAND_TYPE t, COMMAND_PAYLOAD p);

bool is_nav_command(command_t *c);

command_t *append_command(command_stack_t *cs, command_t *c);

command_t *pop_command(command_stack_t *cs);

#endif
