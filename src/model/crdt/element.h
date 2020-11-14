#ifndef CRDT_ELEMENT_H
#define CRDT_ELEMENT_H
#include <stdarg.h>
#include <stdbool.h>

typedef struct {
  unsigned int key;      // bit representation of the key.
  unsigned int user_ids; // user ids associated with each key token.
  unsigned int depth;    // length of the key.
  unsigned int version;  // the version of the element.
  void* value;           // value of the element.
} element;

unsigned int key_from_tokens(unsigned int depth, ...);
unsigned int user_ids_from_ids(unsigned int depth, ...);
int key_compare(element* l, element* r);
bool key_equal(element* l, element* r);
void set_key_tokens(element* e, unsigned int depth, ...);

#endif

