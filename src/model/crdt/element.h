#ifndef ELEMENT_H
#define ELEMENT_H

typedef struct {
  unsigned int key;   // bit representation of the key.
  unsigned int depth; // length of the key.
  void* value;        // value of the element.
} element;

unsigned int key_from_tokens(unsigned int depth, ...);
void set_key_tokens(element* e, unsigned int depth, ...);

#endif

