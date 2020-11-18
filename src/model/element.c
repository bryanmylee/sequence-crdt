#include "element.h"
#include "guid.h"

/**
 * @brief Initialize an Element.
 *
 * @param e A pointer to the Element to initialize.
 */
void element_init(Element* e) {
  guid_init(&e->id);
  e->version = 0;
  e->data.ptr = NULL;
}

/**
 * @brief Initialize and allocate a new Element to memory.
 *
 * @return A pointer to the allocated Element.
 */
Element* element_new(void) {
  Element* new = malloc(sizeof(Element));
  element_init(new);
  return new;
}

/**
 * @brief Free an allocated Element with value-type data.
 *
 * @param e A pointer to a pointer to the allocated Element.
 */
void element_free(Element** e) {
  free(*e);
  *e = NULL;
}

/**
 * @brief Free an allocated Element with memory-allocated data, while cleaning
 *        up any internal data.
 *
 * @param e A pointer to a pointer to the allocated Element.
 */
void element_free_ptr(Element** e) {
  element_free_internal_ptr(*e);
  free(*e);
  *e = NULL;
}


/**
 * @brief Free up any internal data of an Element with memory-allocated data.
 *
 * @param e A pointer to the Element.
 */
void element_free_internal_ptr(Element* e) {
  free(e->data.ptr);
  e->data.ptr = NULL;
}

