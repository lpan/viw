#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../src/commands.h"

static void test_history_stack(void) {
  history_stack_t *hs = init_history_stack();
  COMMAND_PAYLOAD p;

  COMMAND_TYPE t1 = HANDLE_APPEND_ROW;
  COMMAND_TYPE t2 = HANDLE_DELETE_CHAR;
  COMMAND_TYPE t3 = HANDLE_DELETE_ROW;

  command_t *c1 = init_command(t1, p);
  command_t *c2 = init_command(t2, p);
  command_t *c3 = init_command(t3, p);

  append_command(hs, c1);
  assert(hs->top->type == t1);
  assert(hs->bottom->type == t1);
  assert(c1->prev == NULL);
  assert(c1->next == NULL);
  assert(c2->prev == NULL);
  assert(c2->next == NULL);
  assert(c3->prev == NULL);
  assert(c3->next == NULL);

  append_command(hs, c2);
  assert(hs->top == c2);
  assert(hs->bottom == c1);
  assert(c1->prev == NULL);
  assert(c1->next == c2);
  assert(c2->prev == c1);
  assert(c2->next == NULL);
  assert(c3->prev == NULL);
  assert(c3->next == NULL);

  append_command(hs, c3);
  assert(hs->top == c3);
  assert(hs->bottom == c1);
  assert(c1->prev == NULL);
  assert(c1->next == c2);
  assert(c2->prev == c1);
  assert(c2->next == c3);
  assert(c3->prev == c2);
  assert(c3->next == NULL);

  command_t *tmp = pop_command(hs);
  assert(tmp == c3);
  assert(hs->top == c2);
  assert(hs->bottom == c1);
  assert(c1->prev == NULL);
  assert(c1->next == c2);
  assert(c2->prev == c1);
  assert(c2->next == NULL);
  assert(c3->prev == NULL);
  assert(c3->next == NULL);
  free(tmp);

  tmp = pop_command(hs);
  assert(tmp == c2);
  assert(hs->top == c1);
  assert(hs->bottom == c1);
  assert(c1->prev == NULL);
  assert(c1->next == NULL);
  assert(c2->prev == NULL);
  assert(c2->next == NULL);
  free(tmp);

  tmp = pop_command(hs);
  assert(tmp == c1);
  assert(hs->top == NULL);
  assert(hs->bottom == NULL);
  assert(c1->prev == NULL);
  assert(c1->next == NULL);
  free(tmp);

  destroy_history_stack(hs);
}

int main(void) {
  test_history_stack();

  return 0;
}
