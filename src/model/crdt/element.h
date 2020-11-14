#ifndef CRDT_ELEMENT_H
#define CRDT_ELEMENT_H
#include "guid.h"

typedef struct {
  guid id;
  unsigned int version;
  void* value;
} element;

#endif

