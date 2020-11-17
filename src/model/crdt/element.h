#ifndef CRDT_ELEMENT_H
#define CRDT_ELEMENT_H
#include <stddef.h>
#include <stdlib.h>
#include "guid.h"

typedef union {
  char value;
  void* ptr;
} EData;

typedef struct {
  Guid id;
  unsigned int version;
  EData data;
} Element;

void element_init(Element* e);
Element* element_new(void);
void element_free(Element** e);
void element_free_ptr(Element** e);
void element_free_internal_ptr(Element* e);

#endif

