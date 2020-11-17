#include "guid.h"
#include "element.h"

void element_init(Element* e) {
  guid_init(&e->id);
  e->version = 0;
  e->data.ptr = NULL;
}

Element* element_new(void) {
  Element* new = malloc(sizeof(Element));
  element_init(new);
  return new;
}

void element_free(Element** e) {
  free(*e);
  *e = NULL;
}

void element_free_ptr(Element** e) {
  element_free_internal_ptr(*e);
  free(*e);
  *e = NULL;
}

void element_free_internal_ptr(Element* e) {
  free(e->data.ptr);
  e->data.ptr = NULL;
}

