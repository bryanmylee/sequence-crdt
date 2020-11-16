#include "guid.h"
#include "element.h"

void element_init(Element* e) {
  guid_init(&e->id);
  e->version = 0;
  e->value = NULL;
}

Element* element_new(void) {
  Element* new = malloc(sizeof(Element));
  element_init(new);
  return new;
}

void element_free(Element** e) {
  element_free_internal(*e);
  free(*e);
  *e = NULL;
}

void element_free_internal(Element* e) {
  free(e->value);
}

