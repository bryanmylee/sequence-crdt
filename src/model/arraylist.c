#include "arraylist.h"

#define INITIAL_CAPACITY 16
#define EXPAND_FACTOR 2

/**
 * @brief Increase the capacity of an existing arraylist.
 *
 * @param al A pointer to the arraylist to expand.
 */
void _al_expand(arraylist* al) {
  al->cap *= EXPAND_FACTOR;
  al->data = realloc(al->data, al->cap * sizeof(void*));
}

/**
 * @brief Increase the capacity of an existing arraylist until it is greater
 * than or equal to a minimum capacity.
 *
 * @param al A pointer to the arraylist to expand.
 * @param min The minimum capacity to expand to.
 */
void _al_expand_to_min(arraylist* al, unsigned int min) {
  while (al->cap < min) {
    al->cap *= EXPAND_FACTOR;
  }
  al->data = realloc(al->data, al->cap * sizeof(void*));
}

/**
 * @brief Initialize an arraylist.
 *
 * @param al A pointer to the arraylist to initialize.
 */
void al_init(arraylist* al) {
  al->cap = INITIAL_CAPACITY;
  al->size = 0;
  al->data = malloc(al->cap * sizeof(void*));
}

/**
 * @brief Add an element to the arraylist at a specified index.
 *
 * @param al A pointer to the arraylist to add to.
 * @param e A pointer to the element to add. The element must be alloc-ed.
 * @param index The index at which to add the element.
 *
 * @return If the add was successful, returns true.
 */
bool al_add_at(arraylist* al, void* e, unsigned int index) {
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
 * @brief Add an element to the end of the arraylist.
 *
 * @param al A pointer to the arraylist to add to.
 * @param e A pointer to the element to add. The element must be alloc-ed.
 *
 * @return If the add was successful, returns true.
 */
bool al_add(arraylist* al, void* e) {
  return al_add_at(al, e, al->size);
}

/**
 * @brief Add an array of elements to the arraylist at a specified index.
 *
 * This method is more efficient than adding each element sequentially, as it
 * swaps each shifted element only once.
 *
 * @param al A pointer to the arraylist to add to.
 * @param es A pointer to the array of elements to add. The elements must each be alloc-ed.
 * @param n The size of the elements.
 * @param index The index at which to add the element.
 *
 * @return If the add was successful, returns true.
 */
bool al_add_all_at(arraylist* al, void** es, unsigned int n, unsigned int index) {
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
 * @brief Add an array of elements to the end of the arraylist.
 *
 * @param al A pointer to the arraylist to add to.
 * @param es A pointer to the array of elements to add. The elements must each be alloc-ed.
 * @param n The size of the elements.
 *
 * @return If the add was successful, returns true.
 */
bool al_add_all(arraylist* al, void** es, unsigned int n) {
  return al_add_all_at(al, es, n, al->size);
}

/**
 * @brief Remove an element from the arraylist at a given index.
 *
 * @param al A pointer to the arraylist to remove from.
 * @param index The index to remove the element from.
 *
 * @return A pointer to the removed element.
 */
void* al_remove_at(arraylist* al, unsigned int index) {
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

