#ifndef CRDT_ELEMENT_H
#define CRDT_ELEMENT_H
#include "guid.h"

typedef struct {
  Guid id;
  unsigned int version;
  void* value;
} Element;

#endif

