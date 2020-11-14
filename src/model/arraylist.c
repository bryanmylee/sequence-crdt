#include <stdlib.h>
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

void al_init(arraylist* al) {
  al->cap = INITIAL_CAPACITY;
  al->size = 0;
  al->data = malloc(al->cap * sizeof(void*));
}

void al_add(arraylist* al, void* e) {
  if (al->size == al->cap) {
    _al_expand(al);
  }
  al->data[al->size] = e;
  al->size++;
}

