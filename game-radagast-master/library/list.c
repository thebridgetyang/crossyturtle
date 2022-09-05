#include "list.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

const double RESIZE_CONST = 2.0;

typedef struct list {
  void **data;
  size_t num_elements; // occupied len
  size_t max_len;      // total size of list
  free_func_t freer;
} list_t;

list_t *list_init(size_t initial_size, free_func_t freer) {
  list_t *list = malloc(sizeof(*list));
  assert(list != NULL);
  list->data = malloc(((initial_size < 1) ? 1 : initial_size) * sizeof(void *));
  assert(list->data != NULL);
  list->num_elements = 0;
  list->max_len = (initial_size < 1) ? 1 : initial_size;
  list->freer = freer;
  return list;
}

void list_free(list_t *list) {
  if (list->freer != NULL) {
    for (size_t i = 0; i < list->num_elements; i++) {
      list->freer(list->data[i]);
    }
  }
  free(list->data);
  free(list);
}

size_t list_size(list_t *list) { return list->num_elements; }

void *list_get(list_t *list, size_t index) {
  assert(index < list_size(list));
  return list->data[index];
}

void *list_remove(list_t *list, size_t index) {
  assert(index < list->num_elements);
  void *temp = list->data[index];
  for (size_t i = index; i < list->num_elements - 1; i++) {
    list->data[i] = list->data[i + 1];
  }
  list->num_elements -= 1;

  return temp;
}

void list_add(list_t *list, void *value) {
  list_insert(list, value, list->num_elements);
}

void list_insert(list_t *list, void *value, size_t index) {
  assert(index <= list->num_elements);
  list_resize(list);
  for (size_t i = list->num_elements; i > index; i--) {
    list->data[i] = list->data[i - 1];
  }
  list->data[index] = value;
  list->num_elements += 1;
}

void *list_set(list_t *list, void *value, size_t index) {
  assert(value != NULL);
  assert(index < list->num_elements);
  void *temp = list->data[index];
  list->data[index] = value;
  return temp;
}

void list_resize(list_t *list) {
  assert(list->num_elements <= list->max_len);
  if (list->num_elements == list->max_len) {
    list->max_len = (size_t)(RESIZE_CONST * list->max_len);
    list->data = realloc(list->data, list->max_len * sizeof(void *));
  }
  assert(list->data != NULL);
}