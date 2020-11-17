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
 * @param al     A pointer to the ArrayList to add to.
 * @param to_add A pointer to the element to add.
 * @param index  The index at which to add the element.
 *
 * @return If the add was successful, returns true.
 */
bool al_add_at(ArrayList* al, void* to_add, unsigned int index) {
  if (index < 0 || index > al->size) {
    return false;
  }
  if (al->size == al->cap) {
    _al_expand(al);
  }
  // move right elements to the right.
  char* move_dst = al->data + (index + 1) * al->esize;
  char* move_src = al->data + index * al->esize;
  int n_moved = al->size - index;
  memmove(move_dst, move_src, n_moved * al->esize);
  // copy element value into the arraylist.
  memcpy(move_src, to_add, al->esize);
  al->size++;
  return true;
}

/**
 * @brief Add an element to the end of the ArrayList.
 *
 * @param al     A pointer to the ArrayList to add to.
 * @param to_add A pointer to the element to add.
 *
 * @return If the add was successful, returns true.
 */
bool al_add(ArrayList* al, void* to_add) {
  return al_add_at(al, to_add, al->size);
}

/**
 * @brief Add an array of elements to the ArrayList at a specified index.
 *
 *        This method is more efficient than adding each element sequentially,
 *        as it swaps each shifted element only once.
 *
 * @param al      A pointer to the ArrayList to add to.
 * @param to_adds A pointer to the array of elements to add.
 * @param n       The size of the elements.
 * @param index   The index at which to add the element.
 *
 * @return If the add was successful, returns true.
 */
bool al_add_all_at(ArrayList* al, void* to_adds, unsigned int n, unsigned int index) {
  if (index < 0 || index > al->size) {
    return false;
  }
  _al_expand_to_min(al, al->size + n);
  // move right elements to the right.
  char* move_dst = al->data + (index + n) * al->esize;
  char* move_src = al->data + index * al->esize;
  int n_moved = al->size - index;
  memmove(move_dst, move_src, n_moved * al->esize);
  // copy element values into the arraylist.
  memcpy(move_src, to_adds, n * al->esize);
  al->size += n;
  return true;
}

/**
 * @brief Add an array of elements to the end of the ArrayList.
 *
 * @param al      A pointer to the ArrayList to add to.
 * @param to_adds A pointer to the array of elements to add. The elements must
 *                each be alloc-ed.
 * @param n       The size of the elements.
 *
 * @return If the add was successful, returns true.
 */
bool al_add_all(ArrayList* al, void* to_adds, unsigned int n) {
  return al_add_all_at(al, to_adds, n, al->size);
}

bool _al_remove_at(ArrayList* al, unsigned int index, void* buf) {
  if (index < 0 || index >= al->size) {
    return false;
  }
  if (buf != NULL) {
    memcpy(buf, al->data + index * al->esize, al->esize);
  }
  // move right elements to the left.
  char* dst = al->data + index * al->esize;
  char* src = al->data + (index + 1) * al->esize;
  int n_moved = al->size - index - 1;
  memmove(dst, src, n_moved * al->esize);
  al->size--;
  return true;
}

/**
 * @brief Remove an element from the ArrayList at a given index.
 *
 * @param al    A pointer to the ArrayList to remove from.
 * @param index The index to remove the element from.
 *
 * @return If the remove was successful, returns true.
 */
bool al_remove_at(ArrayList* al, unsigned int index) {
  return _al_remove_at(al, index, NULL);
}

/**
 * @brief Remove an element from the ArrayList at a given index, and store it
 *        in a provided buffer.
 *
 * @param al    A pointer to the ArrayList to remove from.
 * @param index The index to remove the element from.
 * @param buf   A pointer to a buffer to store the removed element in.
 *
 * @return If the remove was successful, returns true.
 */
bool al_remove_at_save(ArrayList* al, unsigned int index, void* buf) {
  return _al_remove_at(al, index, buf);
}

bool _al_remove_all_at(ArrayList* al, unsigned int from, unsigned int to, void* buf) {
  if (from < 0 || from > al->size || to < 0 || to > al->size || from > to) {
    return false;
  }
  int n_remove = to - from;
  if (buf != NULL) {
    // store deleted elements.
    char* copy_src = al->data + from * al->esize;
    memcpy(buf, copy_src, n_remove * al->esize);
  }
  // move right elements to the left.
  char* move_dst = al->data + from * al->esize;
  char* move_src = al->data + to * al->esize;
  int n_moved = al->size - to;
  memmove(move_dst, move_src, n_moved * al->esize);
  al->size -= n_remove;
  return true;
}

/**
 * @brief Remove multiple consecutive elements from the ArrayList.
 *
 * @param al   A pointer to the ArrayList to remove from.
 * @param from The index of the first element to remove inclusive.
 * @param to   The index of the last element to remove exclusive.
 *
 * @return If the remove is successful, returns true.
 */
bool al_remove_all_at(ArrayList* al, unsigned int from, unsigned int to) {
  return _al_remove_all_at(al, from, to, NULL);
}

/**
 * @brief Remove multiple consecutive elements from the ArrayList.
 *
 * @param al   A pointer to the ArrayList to remove from.
 * @param from The index of the first element to remove inclusive.
 * @param to   The index of the last element to remove exclusive.
 * @param buf  A pointer to an array to store the removed elements in. The
 *             array must be large enough to store all removed elements.
 *
 * @return If the remove is successful, returns true.
 */
bool al_remove_all_at_save(ArrayList* al, unsigned int from, unsigned int to, void* buf) {
  return _al_remove_all_at(al, from, to, buf);
}

