#ifndef CRDT_ELEMENT_H
#define CRDT_ELEMENT_H
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "guid.h"

typedef enum {
  EVALUE,
  EREF,
} EType;

typedef union {
  long value;
  void *ptr;
} EData;

typedef struct {
  Guid id;              // the globally unique id of the element.
  unsigned int version; // the version of the Sequence which created the element.
  EType type;           // the type of data stored.
  EData data;           // the data stored, which could be a value or a pointer.
  size_t data_size;
} Element;

void element_init(Element *e);
Element *element_new(void);
void element_free(Element **e);
void element_free_internal(Element *e);
void *element_get_ptr(Element *e);
long element_get_value(Element *e);
void element_set_ptr(Element *e, void *ptr, size_t data_size);
void element_set_value(Element *e, long value);
void element_new_deep_copy(Element *dst, Element *src);

#endif

