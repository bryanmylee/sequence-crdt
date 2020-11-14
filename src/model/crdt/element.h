#ifndef ELEMENT_H
#define ELEMENT_H

typedef struct {
  unsigned int key;
  void* value;
} element;

unsigned int key_from_tokens(int depth, ...);
void set_key_tokens(element* e, int depth, ...);

#endif

