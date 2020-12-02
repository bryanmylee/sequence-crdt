#include "element.h"

/**
 * @brief Initialize an Element.
 *
 * @param e A pointer to the Element to initialize.
 */
void element_init(Element *e) {
  guid_init(&e->id);
  e->version = 0;
  e->data.ptr = NULL;
}

/**
 * @brief Initialize and allocate a new Element to memory.
 *
 * @return A pointer to the allocated Element.
 */
Element *element_new(void) {
  Element *new = malloc(sizeof(Element));
  element_init(new);
  return new;
}

/**
 * @brief Free up any internal data of an Element with memory-allocated data.
 *
 * @param e A pointer to the Element.
 */
static void element_free_ptr(Element *e) {
  free(e->data.ptr);
  e->data.ptr = NULL;
}

/**
 * @brief Free an allocated Element.
 *
 * @param e A pointer to a pointer to the allocated Element.
 */
void element_free(Element **e) {
  if ((**e).type == EREF) {
    element_free_ptr(*e);
  }
  free(*e);
  *e = NULL;
}

void *element_get_ptr(Element *e) {
  return e->data.ptr;
}

long element_get_value(Element *e) {
  return e->data.value;
}

void element_set_ptr(Element *e, void *ptr) {
  e->type = EREF;
  e->data.ptr = ptr;
}

void element_set_value(Element *e, long value) {
  e->type = EVALUE;
  e->data.value = value;
}

