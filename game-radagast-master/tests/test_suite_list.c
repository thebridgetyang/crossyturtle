#include "list.h"
#include "test_util.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>

void test_list_init_add_get() {
  list_t *test_list = list_init(1, (free_func_t)free);
  int *a = malloc(sizeof(int));
  *a = 1;
  list_add(test_list, (void *)(a));
  assert(*(int *)list_get(test_list, 0) == 1);
  list_free(test_list);
}

void test_list_add_remove_size() {
  list_t *test_list = list_init(5, (free_func_t)free);
  int *a = malloc(sizeof(int));
  *a = 1;
  int *b = malloc(sizeof(int));
  *b = 2;
  int *c = malloc(sizeof(int));
  *c = 3;

  list_add(test_list, (void *)(a));
  list_add(test_list, (void *)(b));
  list_add(test_list, (void *)(c));
  void *remove = list_remove(test_list, 0);
  free(remove);
  assert((int)list_size(test_list) == 2);
  list_free(test_list);
}

int main(int argc, char *argv[]) {
  // Run all tests? True if there are no command-line arguments
  bool all_tests = argc == 1;
  // Read test name from file
  char testname[100];
  if (!all_tests) {
    read_testname(argv[1], testname, sizeof(testname));
  }

  DO_TEST(test_list_init_add_get)
  DO_TEST(test_list_add_remove_size)

  puts("list_test PASS");
}