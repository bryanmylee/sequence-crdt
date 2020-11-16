#ifndef ARRAYLIST_H
#define ARRAYLIST_H
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
  unsigned int cap;
  unsigned int size;
  void** data;
} ArrayList;

void al_init(ArrayList* al);
ArrayList* al_new(void);
void al_free(ArrayList** al);
void al_free_internal(ArrayList* al);
bool al_add_at_static(ArrayList* al, void* e, size_t esize, unsigned int index);
bool al_add_static(ArrayList* al, void* e, size_t esize);
bool al_add_at(ArrayList* al, void* e, unsigned int index);
bool al_add(ArrayList* al, void* e);
bool al_add_all_at(ArrayList* al, void** es, unsigned int n, unsigned int index);
bool al_add_all(ArrayList* al, void** es, unsigned int n);
void* al_remove_at(ArrayList* al, unsigned int index);
void al_remove_all_at(ArrayList* al, void** es, unsigned int from, unsigned int to);

#endif
