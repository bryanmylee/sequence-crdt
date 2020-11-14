#ifndef CRDT_ELEMENT_H
#define CRDT_ELEMENT_H
#include <stdarg.h>
#include <stdbool.h>

typedef struct {
  unsigned long key;      // bit representation of the key, up to 6 tokens.
  unsigned long user_ids; // user ids associated with each key token.
  int depth;              // length of the key.
  unsigned int version;   // the version of the element.
  void* value;            // value of the element.
} element;

unsigned long key_from_tokens(int depth, ...);
unsigned long user_ids_from_ids(int depth, ...);
int key_compare(element* l, element* r);
bool key_equal(element* l, element* r);
void set_key_tokens(element* e, int depth, ...);

#endif

