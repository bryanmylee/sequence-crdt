#include "arraylist.h"

#define INITIAL_CAPACITY 16
#define EXPAND_FACTOR 2

/**
 * @brief Increase the capacity of an existing arraylist.
 *
 * @param a The arraylist to expand.
 */
void _al_expand(arraylist* al) {
  al->cap *= EXPAND_FACTOR;
  al->data = realloc(al->data, al->cap * sizeof(void*));
}

void _al_expand_to_min(arraylist* al, unsigned int min) {
  while (al->cap < min) {
    al->cap *= EXPAND_FACTOR;
  }
  al->data = realloc(al->data, al->cap * sizeof(void*));
}

void al_init(arraylist* al) {
  al->cap = INITIAL_CAPACITY;
  al->size = 0;
  al->data = malloc(al->cap * sizeof(void*));
}

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

bool al_add(arraylist* al, void* e) {
  return al_add_at(al, e, al->size);
}

bool al_add_all_at(arraylist* al, void** es, unsigned int n, unsigned int index) {
  if (index < 0 || index > al->size) {
    return false;
  }
  _al_expand_to_min(al, al->size + n);
  // shift right elements.
  for (unsigned int i = al->size - 1; i >= index; i--) {
    al->data[i + n] = al->data[i];
  }
  // copy elements.
  for (unsigned int i = 0; i < n; i++) {
    al->data[index + i] = es[i];
  }
  al->size += n;
  return true;
}

bool al_add_all(arraylist* al, void** es, unsigned int n) {
  return al_add_all_at(al, es, n, al->size);
}

