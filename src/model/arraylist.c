#include <string.h>
#include "arraylist.h"

#define INITIAL_CAPACITY 16
#define EXPAND_FACTOR 2

/**
 * @brief Initialize an ArrayList.
 *
 * @param al A pointer to the ArrayList to initialize.
 */
void al_init(ArrayList* al) {
  al->cap = INITIAL_CAPACITY;
  al->size = 0;
  al->data = malloc(al->cap * sizeof(void*));
}

ArrayList* al_new(void) {
  ArrayList* new = malloc(sizeof(ArrayList));
  al_init(new);
  return new;
}

void al_free(ArrayList** al) {
  al_free_internal(*al);
  free(*al);
  *al = NULL;
}

void al_free_internal(ArrayList* al) {
  for (unsigned int i = 0; i < al->size; i++) {
    free(al->data[i]);
    *(al->data + i) = NULL;
  }
}

void al_free_with_cleanup(ArrayList** al, void (*fn)(void* e)) {
  al_free_internal_with_cleanup(*al, fn);
  free(*al);
  *al = NULL;
}

void al_free_internal_with_cleanup(ArrayList* al, void (*fn)(void* e)) {
  for (unsigned int i = 0; i < al->size; i++) {
    (*fn)(al->data[i]);
    free(al->data[i]);
    *(al->data + i) = NULL;
  }
}

/**
 * @brief Increase the capacity of an existing ArrayList.
 *
 * @param al A pointer to the ArrayList to expand.
 */
void _al_expand(ArrayList* al) {
  al->cap *= EXPAND_FACTOR;
  al->data = realloc(al->data, al->cap * sizeof(void*));
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
  al->data = realloc(al->data, al->cap * sizeof(void*));
}

/**
 * @brief Add an element that is not allocated to heap to the ArrayList at a
 * specified index.
 *
 * @param al    A pointer to the ArrayList to add to.
 * @param e     A pointer to the element to add.
 * @param esize The size of the element to add.
 * @param index The index at which to add the element.
 *
 * @return If the add was successful, returns true.
 */
bool al_add_at_static(ArrayList* al, void* e, size_t esize, unsigned int index) {
  if (index < 0 || index > al->size) {
    return false;
  }
  if (al->size == al->cap) {
    _al_expand(al);
  }
  // shift right elements.
  for (unsigned int i = al->size; i > index; i--) {
    al->data[i] = al->data[i - 1];
  }
  al->data[index] = malloc(esize);
  memcpy(al->data[index], e, esize);
  al->size++;
  return true;
}

/**
 * @brief Add an element that is not allocated to heap to the end of the
 * ArrayList.
 *
 * @param al    A pointer to the ArrayList to add to.
 * @param e     A pointer to the element to add.
 * @param esize The size of the element to add.
 *
 * @return If the add was successful, returns true.
 */
bool al_add_static(ArrayList* al, void* e, size_t esize) {
  return al_add_at_static(al, e, esize, al->size);
}

/**
 * @brief Add an element to the ArrayList at a specified index.
 *
 * @param al    A pointer to the ArrayList to add to.
 * @param e     A pointer to the alloc-ed element to add.
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
  // shift right elements.
  for (unsigned int i = al->size; i > index; i--) {
    al->data[i] = al->data[i - 1];
  }
  al->data[index] = e;
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
 * @param es    A pointer to the array of elements to add. The elements must
 *              each be alloc-ed.
 * @param n     The size of the elements.
 * @param index The index at which to add the element.
 *
 * @return If the add was successful, returns true.
 */
bool al_add_all_at(ArrayList* al, void** es, unsigned int n, unsigned int index) {
  if (index < 0 || index > al->size) {
    return false;
  }
  _al_expand_to_min(al, al->size + n);
  // shift right elements.
  for (unsigned int i = al->size; i > index; i--) {
    al->data[i - 1 + n] = al->data[i - 1];
  }
  // copy elements.
  for (unsigned int i = 0; i < n; i++) {
    al->data[index + i] = es[i];
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
bool al_add_all(ArrayList* al, void** es, unsigned int n) {
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
  if (index < 0 || index > al->size) {
    return NULL;
  }
  void* to_return = al->data[index];
  // shift right elements.
  for (int i = index; i < al->size; i++) {
    al->data[i] = al->data[i + 1];
  }
  al->size--;
  return to_return;
}

/**
 * @brief Remove multiple consecutive elements from the ArrayList.
 *
 * @param al   A pointer to the ArrayList to remove from.
 * @param es   A pointer to an array to store the removed elements in. The
 *             array must be large enough to store all removed elements.
 * @param from The index of the first element to remove inclusive.
 * @param to   The index of the last element to remove exclusive.
 */
void al_remove_all_at(ArrayList* al, void** es, unsigned int from, unsigned int to) {
  if (from < 0 || from > al->size || to < 0 || to > al->size || from > to) {
    return;
  }
  int n = to - from;
  // store deleted elements.
  for (int i = 0; i < n; i++) {
    es[i] = al->data[from + i];
  }
  // shift right elements.
  for (int i = to; i < al->size; i++) {
    al->data[i - n] = al->data[i];
  }
  al->size -= n;
}

