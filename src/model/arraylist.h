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
bool al_add(arraylist* al, void* e);
bool al_add_at(arraylist* al, void* e, unsigned int index);

#endif
