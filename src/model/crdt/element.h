#ifndef ELEMENT_H
#define ELEMENT_H

typedef struct element element;

unsigned int key_from_tokens(int depth, ...);
void set_key_tokens(element* e, int depth, ...);

#endif

