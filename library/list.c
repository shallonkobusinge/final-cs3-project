#include "list.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

const int GROWTH_FACTOR = 2; // List

void list_free(list_t *list)
{
  assert(list != NULL);
  if (list->freer != NULL)
  {
    for (size_t i = 0; i < list->size; i++)
    {
      list->freer(list->data[i]);
    }
  }
  free(list->data);
  free(list);
}

list_t *list_init(size_t initial_size, free_func_t freer)
{
  list_t *list = malloc(sizeof(list_t));
  list->size = 0;
  assert(list->capacity != 0);
  list->capacity = initial_size;
  list->data = malloc(sizeof(void *) * initial_size);
  list->freer = freer;
  return list;
}

list_t *list_copy(list_t *original)
{
  assert(original != NULL);
  list_t *copy = list_init(original->capacity, original->freer);

  for (size_t i = 0; i < original->size; i++)
  {
    vector_t *original_vector = list_get(original, i);
    vector_t *new_vector = malloc(sizeof(vector_t));
    assert(new_vector != NULL);
    *new_vector = *original_vector;
    list_add(copy, new_vector);
  }
  return copy;
}

size_t list_size(list_t *list)
{
  assert(list != NULL);
  return list->size;
}

void *list_get(list_t *list, size_t index)
{
  assert(list != NULL);
  assert(index >= 0 && index < list->size);

  return list->data[index];
}

void list_add(list_t *list, void *value)
{
  assert(list != NULL);
  assert(list->data);
  assert(list->size <= list->capacity);
  assert(value != NULL);

  if (list->size == list->capacity)
  {
    size_t new_capacity = list->capacity * GROWTH_FACTOR;
    void **new_data = realloc(list->data, new_capacity * sizeof(void *));
    list->data = new_data;
    list->capacity = new_capacity;
  }

  list->data[list->size] = value;

  list->size++;
}

void *list_remove(list_t *list, size_t index)
{
  assert(list->size > 0);

  void *t = list->data[index];
  for (size_t i = index; i < list->size - 1; i++)
  {
    list->data[i] = list->data[i + 1];
  }
  list->size--;
  return t;
}
