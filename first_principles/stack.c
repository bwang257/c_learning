/*
Custom exercise defining a stack in C to hold integers

Compile and run:
gcc stack.c -o run -Wall -Wextra
./run
leaks --atExit -- ./run
*/


#include <stdio.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>


typedef struct node node;
struct node {
  int val;
  node* next;
};
static_assert(sizeof(node) == 16, "");

typedef struct stack stack;
struct stack {
  node* data;
  size_t len;
};
static_assert(sizeof(stack) == 16, "");

stack* stack_new(){
  stack* new_stack = malloc(sizeof(stack));
  if (!new_stack) return NULL;

  // init the stack
  new_stack->data = NULL;
  new_stack->len = 0;
  return new_stack;
}

int stack_push(stack* s, int val){
  if (!s){
    fprintf(stderr, "stack push called on null ptr\n");
    return -1;
  }
  node* new_node = malloc(sizeof(node));
  if (!new_node) return -1;
  new_node->val = val;
  new_node->next = s->data;
  s->data = new_node;
  s->len += 1;
  return 0;
}

bool stack_empty(stack* s){
  if (!s){
    fprintf(stderr, "stack empty called on null ptr\n");
    return 0; // technically empty
  }
  return s->len == 0;
}

int stack_top(stack* s, int* out){
  if (!s){
    fprintf(stderr, "stack top called on null ptr\n");
    return -1;
  }
  if (s->data){
    *out = s->data->val;
    return 0;
  } else {
    fprintf(stderr, "Stack top called on empty stack\n");
    return -1;
  }
}

int stack_pop(stack* s){
  if (!s){
    fprintf(stderr, "stack pop called on null ptr\n");
    return -1;
  }
  if (stack_empty(s)){
    fprintf(stderr, "Pop called on empty stack!\n");
    return -1;
  } else {
    node* temp = s->data;
    s->data = s->data->next;
    free(temp);
    s->len -=1;
    return 0;
  }
}

void stack_clear(stack* s){
  if (!s){
    fprintf(stderr, "stack cleared called on null ptr\n");
    return;
  }
  node* head = s->data;
  while (head){
    node* temp = head->next;
    free(head);
    head = temp;
  }
  s->data = NULL;
  s->len = 0;
  return;
}

void stack_destroy(stack* s){
  if (!s) return;
  node* head = s->data;
  while (head){
    node* temp = head->next;
    free(head);
    head = temp;
  }
  free(s);
  return;
}


// ---------------------------------------------------------------------------------------------
// Claude tests
// CHECK records a failure and keeps going (unlike assert), so one run lists every failing line.
// ---------------------------------------------------------------------------------------------
#include <ctype.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>

static int fails = 0;
#define CHECK(c) do { if (!(c)) { printf("  FAIL line %d: %s\n", __LINE__, #c); fails++; } } while (0)

// Error-message testing: point fd 2 (stderr) at a temp file, run the call, read the file back.
// The text comes back lowercased so tests match on keywords ("pop", "empty", "null"), not exact wording.
static int saved_fd;
static FILE *cap_file;
static char cap_buf[256];

static void cap_start(void){
  fflush(stderr);
  saved_fd = dup(2);
  cap_file = tmpfile();
  dup2(fileno(cap_file), 2);
}

static const char *cap_stop(void){
  fflush(stderr);
  dup2(saved_fd, 2);
  close(saved_fd);
  rewind(cap_file);
  size_t n = fread(cap_buf, 1, sizeof cap_buf - 1, cap_file);
  cap_buf[n] = '\0';
  fclose(cap_file);
  for (size_t i = 0; i < n; ++i) cap_buf[i] = (char)tolower((unsigned char)cap_buf[i]);
  return cap_buf;
}

static void test_basic(void){
  stack *s = stack_new();
  int out = 0;
  CHECK(s && stack_empty(s) && s->len == 0);
  CHECK(stack_push(s, 5) == 0 && stack_push(s, 6) == 0);
  CHECK(!stack_empty(s) && s->len == 2);
  CHECK(stack_top(s, &out) == 0 && out == 6);
  CHECK(stack_top(s, &out) == 0 && out == 6);                      // top must not remove
  CHECK(stack_pop(s) == 0 && stack_top(s, &out) == 0 && out == 5);
  CHECK(stack_pop(s) == 0 && stack_empty(s) && s->len == 0);
  stack_destroy(s);
}

static void test_values(void){  // includes -1, the old in-band "error" value
  stack *s = stack_new();
  int vals[] = {INT_MIN, -1, 0, 1, INT_MAX}, out = 0;
  for (int i = 0; i < 5; i++) CHECK(stack_push(s, vals[i]) == 0);
  for (int i = 4; i >= 0; i--) {
    CHECK(stack_top(s, &out) == 0 && out == vals[i]);
    CHECK(stack_pop(s) == 0);
  }
  CHECK(stack_empty(s));
  stack_destroy(s);
}

static void test_empty_errors(void){
  stack *s = stack_new();
  int out = 12345, r;
  const char *m;
  cap_start(); r = stack_pop(s); m = cap_stop();
  CHECK(r == -1);
  CHECK(strstr(m, "pop") && strstr(m, "empty"));
  CHECK(s->len == 0);                                              // failed pop must not underflow len
  cap_start(); r = stack_top(s, &out); m = cap_stop();
  CHECK(r == -1 && out == 12345);                                  // *out untouched on failure
  CHECK(strstr(m, "top") && strstr(m, "empty"));
  stack_destroy(s);
}

static void test_null_errors(void){
  int out = 12345, r;
  const char *m;
  cap_start(); r = stack_push(NULL, 1); m = cap_stop();
  CHECK(r == -1 && strstr(m, "null") && strstr(m, "push"));
  cap_start(); r = stack_pop(NULL); m = cap_stop();
  CHECK(r == -1 && strstr(m, "null") && strstr(m, "pop"));
  cap_start(); r = stack_top(NULL, &out); m = cap_stop();
  CHECK(r == -1 && out == 12345 && strstr(m, "null") && strstr(m, "top"));
  cap_start(); stack_clear(NULL); m = cap_stop();
  CHECK(strstr(m, "null") && strstr(m, "clear"));
  cap_start(); (void)stack_empty(NULL); m = cap_stop();            // design question: is NULL "empty"?
  CHECK(strstr(m, "null") && strstr(m, "empty"));
  cap_start(); stack_destroy(NULL); m = cap_stop();
  CHECK(m[0] == '\0');                                             // destroy(NULL) is a silent no-op
}

static void test_scale_and_order(void){
  stack *s = stack_new();
  int out = 0, ok = 1;
  for (int i = 0; i < 10000; i++) ok &= (stack_push(s, i) == 0);
  CHECK(ok && s->len == 10000);
  for (int i = 9999; i >= 0; i--) {
    if (stack_top(s, &out) != 0 || out != i) { CHECK(0); break; }
    stack_pop(s);
  }
  CHECK(stack_empty(s) && s->len == 0);
  stack_destroy(s);
}

static void test_interleave(void){
  stack *s = stack_new();
  int out = 0;
  stack_push(s, 1); stack_push(s, 2);
  CHECK(stack_pop(s) == 0);
  stack_push(s, 3);
  CHECK(stack_top(s, &out) == 0 && out == 3);
  CHECK(stack_pop(s) == 0 && stack_top(s, &out) == 0 && out == 1);
  CHECK(stack_pop(s) == 0 && stack_empty(s));
  CHECK(stack_push(s, 9) == 0 && stack_top(s, &out) == 0 && out == 9 && s->len == 1);   // reuse after emptying
  stack_destroy(s);                                                // non-empty destroy: leaks will tell
}

static void test_clear(void){  // keep this one last: if clear leaves a dangling pointer, ASan aborts here
  stack *s = stack_new();
  int out = 12345, r;
  stack_push(s, 1); stack_push(s, 2); stack_push(s, 3);
  stack_clear(s);
  CHECK(stack_empty(s) && s->len == 0);
  CHECK(s->data == NULL);                                          // freed nodes must not stay reachable
  r = stack_top(s, &out);                                          // not captured: a crash report must stay visible
  CHECK(r == -1 && out == 12345);                                  // top after clear: empty, not freed memory
  CHECK(stack_push(s, 7) == 0 && stack_top(s, &out) == 0 && out == 7 && s->len == 1);
  CHECK(stack_pop(s) == 0 && stack_empty(s));
  stack_destroy(s);                                                // double free if clear left a dangling pointer
}

#define RUN(t) do { int before = fails; t(); printf("%s %s\n", fails == before ? "ok  " : "FAIL", #t); } while (0)
static int run_claude_tests(void){
  setvbuf(stdout, NULL, _IONBF, 0);                                // so FAIL lines survive an ASan abort
  RUN(test_basic);
  RUN(test_values);
  RUN(test_empty_errors);
  RUN(test_null_errors);
  RUN(test_scale_and_order);
  RUN(test_interleave);
  RUN(test_clear);
  return fails;
}

int main(){
  stack* s = stack_new();
  if (!s) return -1;

  // Claude tests
  if (run_claude_tests()) {
    printf("%d CHECK(S) FAILED\n", fails);
    stack_destroy(s);
    return 1;
  }

  puts("ALL TESTS PASSED. NICE!\n");
  free(s);
  return 0;
}



