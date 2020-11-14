#ifndef ARRAYLIST_H
#define ARRAYLIST_H

typedef struct {
  unsigned int cap;
  unsigned int size;
  void** data;
} arraylist;

void al_init(arraylist* al);
void al_add(arraylist* al, void* e);

#endif
