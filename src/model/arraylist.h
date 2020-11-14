#ifndef ARRAYLIST_H
#define ARRAYLIST_H
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
  unsigned int cap;
  unsigned int size;
  void** data;
} arraylist;

void al_init(arraylist* al);
bool al_add_at(arraylist* al, void* e, unsigned int index);
bool al_add(arraylist* al, void* e);
bool al_add_all_at(arraylist* al, void** es, unsigned int n, unsigned int index);
bool al_add_all(arraylist* al, void** es, unsigned int n);
void* al_remove_at(arraylist* al, unsigned int index);

#endif
