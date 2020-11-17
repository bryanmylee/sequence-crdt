#ifndef ARRAYLIST_H
#define ARRAYLIST_H
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
  unsigned int cap;
  unsigned int size;
  char* data;
  size_t esize;
} ArrayList;

void al_init(ArrayList* al, size_t esize);
ArrayList* al_new(size_t esize);
void al_free(ArrayList** al);
void al_free_internal(ArrayList* al);
void* al_get(ArrayList* al, unsigned int index);
void al_set(ArrayList* al, unsigned int index, void* new_value);
bool al_add_at(ArrayList* al, void* e, unsigned int index);
bool al_add(ArrayList* al, void* e);
bool al_add_all_at(ArrayList* al, void* es, unsigned int n, unsigned int index);
bool al_add_all(ArrayList* al, void* es, unsigned int n);
void* al_remove_at(ArrayList* al, unsigned int index);
void al_remove_all_at(ArrayList* al, void* buf, unsigned int from, unsigned int to);

#endif
