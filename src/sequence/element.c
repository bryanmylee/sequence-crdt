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
  element_free_internal(*e);
  free(*e);
  *e = NULL;
}

void element_free_internal(Element *e) {
  guid_free_internal(&e->id);
  if (e->type == EREF) {
    element_free_ptr(e);
  }
}

void *element_get_ptr(Element *e) {
  return e->data.ptr;
}

long element_get_value(Element *e) {
  return e->data.value;
}

void element_set_ptr(Element *e, void *ptr, size_t data_size) {
  e->type = EREF;
  e->data.ptr = ptr;
  e->data_size = data_size;
}

void element_set_value(Element *e, long value) {
  e->type = EVALUE;
  e->data.value = value;
}

void element_new_deep_copy(Element *dst, Element *src) {
  guid_new_copy(&dst->id, &src->id);
  dst->version = src->version;
  dst->type = src->type;
  if (dst->type == EVALUE) {
    dst->data.value = src->data.value;
  } else {
    dst->data_size = src->data_size;
    dst->data.ptr = malloc(dst->data_size);
    memcpy(dst->data.ptr, src->data.ptr, dst->data_size);
  }
}

