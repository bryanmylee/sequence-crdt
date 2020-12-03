#ifndef ARRAYLIST_H
#define ARRAYLIST_H
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  unsigned int cap;  // the maximum capacity at the current allocation size.
  unsigned int size; // the current number of elements.
  char *data;        // a pointer to the backing array of bytes.
  size_t esize;      // the size of each element.
} ArrayList;

void al_init(ArrayList *al, size_t esize);
ArrayList *al_new(size_t esize);
void al_free(ArrayList **al);
void al_free_internal(ArrayList *al);
void al_free_internal_cleanup(ArrayList *al, void (*efree)(void *e));
void *al_get(ArrayList *al, unsigned int index);
void al_set(ArrayList *al, unsigned int index, void *new_value);
bool al_add_at(ArrayList *al, void *to_add, unsigned int index);
bool al_add(ArrayList *al, void *to_add);
bool al_add_all_at(ArrayList *al, void *to_adds, unsigned int n, unsigned int index);
bool al_add_all(ArrayList *al, void *to_adds, unsigned int n);
bool al_remove_at(ArrayList *al, unsigned int index);
bool al_remove_at_save(ArrayList *al, unsigned int index, void *buf);
bool al_remove_all_at(ArrayList *al, unsigned int from, unsigned int to);
bool al_remove_all_at_save(ArrayList *al, unsigned int from, unsigned int to, void *buf);

#endif

