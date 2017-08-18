#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../src/commands.h"

static void test_command_stack(void) {
  command_stack_t *cs = init_command_stack();
  COMMAND_PAYLOAD p;

  COMMAND_TYPE t1 = HANDLE_APPEND_ROW;
  COMMAND_TYPE t2 = HANDLE_DELETE_CHAR;
  COMMAND_TYPE t3 = HANDLE_DELETE_ROW;

  command_t *c1 = init_command(t1, p);
  command_t *c2 = init_command(t2, p);
  command_t *c3 = init_command(t3, p);

  append_command(cs, c1);
  assert(cs->top->type == t1);
  assert(cs->bottom->type == t1);
  assert(c1->prev == NULL);
  assert(c1->next == NULL);
  assert(c2->prev == NULL);
  assert(c2->next == NULL);
  assert(c3->prev == NULL);
  assert(c3->next == NULL);

  append_command(cs, c2);
  assert(cs->top == c2);
  assert(cs->bottom == c1);
  assert(c1->prev == NULL);
  assert(c1->next == c2);
  assert(c2->prev == c1);
  assert(c2->next == NULL);
  assert(c3->prev == NULL);
  assert(c3->next == NULL);

  append_command(cs, c3);
  assert(cs->top == c3);
  assert(cs->bottom == c1);
  assert(c1->prev == NULL);
  assert(c1->next == c2);
  assert(c2->prev == c1);
  assert(c2->next == c3);
  assert(c3->prev == c2);
  assert(c3->next == NULL);

  command_t *tmp = pop_command(cs);
  assert(tmp == c3);
  assert(cs->top == c2);
  assert(cs->bottom == c1);
  assert(c1->prev == NULL);
  assert(c1->next == c2);
  assert(c2->prev == c1);
  assert(c2->next == NULL);
  assert(c3->prev == NULL);
  assert(c3->next == NULL);
  free(tmp);

  tmp = pop_command(cs);
  assert(tmp == c2);
  assert(cs->top == c1);
  assert(cs->bottom == c1);
  assert(c1->prev == NULL);
  assert(c1->next == NULL);
  assert(c2->prev == NULL);
  assert(c2->next == NULL);
  free(tmp);

  tmp = pop_command(cs);
  assert(tmp == c1);
  assert(cs->top == NULL);
  assert(cs->bottom == NULL);
  assert(c1->prev == NULL);
  assert(c1->next == NULL);
  free(tmp);

  destroy_command_stack(cs);
}

int main(void) {
  test_command_stack();

  return 0;
}
