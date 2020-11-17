#include <string.h>
#include "arraylist.h"

#define INITIAL_CAPACITY 16
#define EXPAND_FACTOR 2

/**
 * @brief Initialize an ArrayList.
 *
 * @param al A pointer to the ArrayList to initialize.
 */
void al_init(ArrayList* al, size_t esize) {
  al->cap = INITIAL_CAPACITY;
  al->size = 0;
  al->data = malloc(al->cap * esize);
  al->esize = esize;
}

ArrayList* al_new(size_t esize) {
  ArrayList* new = malloc(sizeof(ArrayList));
  al_init(new, esize);
  return new;
}

void al_free(ArrayList** al) {
  al_free_internal(*al);
  free(*al);
  *al = NULL;
}

void al_free_internal(ArrayList* al) {
  free(al->data);
  al->data = NULL;
}

void* al_get(ArrayList* al, unsigned int index) {
  if (index < 0 || index >= al->size) {
    return NULL;
  }
  return al->data + index * al->esize;
}

void al_set(ArrayList* al, unsigned int index, void* new_value) {
  if (index < 0 || index >= al->size) {
    return;
  }
  char* dst = al->data + index * al->esize;
  memcpy(dst, new_value, al->esize);
}

/**
 * @brief Increase the capacity of an existing ArrayList.
 *
 * @param al A pointer to the ArrayList to expand.
 */
void _al_expand(ArrayList* al) {
  al->cap *= EXPAND_FACTOR;
  al->data = realloc(al->data, al->cap * al->esize);
}

/**
 * @brief Increase the capacity of an existing ArrayList until it is greater
 *        than or equal to a minimum capacity.
 *
 * @param al  A pointer to the ArrayList to expand.
 * @param min The minimum capacity to expand to.
 */
void _al_expand_to_min(ArrayList* al, unsigned int min) {
  while (al->cap < min) {
    al->cap *= EXPAND_FACTOR;
  }
  al->data = realloc(al->data, al->cap * al->esize);
}

/**
 * @brief Add an element to the ArrayList at a specified index.
 *
 * @param al    A pointer to the ArrayList to add to.
 * @param e     A pointer to the element to add.
 * @param index The index at which to add the element.
 *
 * @return If the add was successful, returns true.
 */
bool al_add_at(ArrayList* al, void* e, unsigned int index) {
  if (index < 0 || index > al->size) {
    return false;
  }
  if (al->size == al->cap) {
    _al_expand(al);
  }
  // TODO use a single memmove operation.
  // shift right elements.
  for (unsigned int i = al->size; i > index; i--) {
    char* dst = al->data + i * al->esize;
    char* src = al->data + (i - 1) * al->esize;
    memmove(dst, src, al->esize);
  }
  memcpy(al->data + (index * al->esize), e, al->esize);
  al->size++;
  return true;
}

/**
 * @brief Add an element to the end of the ArrayList.
 *
 * @param al    A pointer to the ArrayList to add to.
 * @param e     A pointer to the element to add.
 * @param esize The size of the element to add.
 *
 * @return If the add was successful, returns true.
 */
bool al_add(ArrayList* al, void* e) {
  return al_add_at(al, e, al->size);
}

/**
 * @brief Add an array of elements to the ArrayList at a specified index.
 *
 *        This method is more efficient than adding each element sequentially,
 *        as it swaps each shifted element only once.
 *
 * @param al    A pointer to the ArrayList to add to.
 * @param es    A pointer to the array of elements to add.
 * @param n     The size of the elements.
 * @param index The index at which to add the element.
 *
 * @return If the add was successful, returns true.
 */
bool al_add_all_at(ArrayList* al, void* es, unsigned int n, unsigned int index) {
  if (index < 0 || index > al->size) {
    return false;
  }
  _al_expand_to_min(al, al->size + n);
  // TODO use a single memmove operation.
  // shift right elements.
  for (unsigned int i = al->size; i > index; i--) {
    char* dst = al->data + (i - 1 + n) * al->esize;
    char* src = al->data + (i - 1) * al->esize;
    memmove(dst, src, al->esize);
  }
  // TODO use a single memcpy operation.
  // copy elements.
  for (unsigned int i = 0; i < n; i++) {
    char* dst = al->data + (index + i) * al->esize;
    void* src = es + i * al->esize;
    memcpy(dst, src, al->esize);
  }
  al->size += n;
  return true;
}

/**
 * @brief Add an array of elements to the end of the ArrayList.
 *
 * @param al A pointer to the ArrayList to add to.
 * @param es A pointer to the array of elements to add. The elements must each
 *           be alloc-ed.
 * @param n  The size of the elements.
 *
 * @return If the add was successful, returns true.
 */
bool al_add_all(ArrayList* al, void* es, unsigned int n) {
  return al_add_all_at(al, es, n, al->size);
}

/**
 * @brief Remove an element from the ArrayList at a given index.
 *
 * @param al    A pointer to the ArrayList to remove from.
 * @param index The index to remove the element from.
 *
 * @return A pointer to the removed element, or NULL if the remove is
 *         unsuccessful.
 */
void* al_remove_at(ArrayList* al, unsigned int index) {
  if (index < 0 || index >= al->size) {
    return NULL;
  }
  void* to_return = al->data + index * al->esize;
  // TODO use a single memmove operation.
  // shift right elements.
  for (int i = index; i < al->size - 1; i++) {
    char* dst = al->data + i * al->esize;
    char* src = al->data + (i + 1) * al->esize;
    memmove(dst, src, al->esize);
  }
  al->size--;
  return to_return;
}

/**
 * @brief Remove multiple consecutive elements from the ArrayList.
 *
 * @param al   A pointer to the ArrayList to remove from.
 * @param buf  A pointer to an array to store the removed elements in. The
 *             array must be large enough to store all removed elements.
 * @param from The index of the first element to remove inclusive.
 * @param to   The index of the last element to remove exclusive.
 */
void al_remove_all_at(ArrayList* al, void* buf, unsigned int from, unsigned int to) {
  if (from < 0 || from > al->size || to < 0 || to > al->size || from > to) {
    return;
  }
  int n = to - from;
  // TODO use a single memcpy operation.
  // store deleted elements.
  for (int i = 0; i < n; i++) {
    char* dst = buf + i * al->esize;
    char* src = al->data + (from + i) * al->esize;
    memcpy(dst, src, al->esize);
  }
  // TODO use a single memmove operation.
  // shift right elements.
  for (int i = to; i < al->size; i++) {
    char* dst = al->data + (i - n) * al->esize;
    char* src = al->data + i * al->esize;
    memmove(dst, src, al->esize);
  }
  al->size -= n;
}

