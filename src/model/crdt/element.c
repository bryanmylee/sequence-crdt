#include <stdarg.h>

typedef struct {
  void* value;
  unsigned int key;
} element;


/**
 * @brief Given a key comprising k tokens, each token doubling in base size
 * in comparison to the previous, we can represent the key as a single
 * unsigned integer value.
 *
 * The first bit holds the value of the first token with size 2.
 * The next two bits hold the values of the next token with size 4.
 * The next three bits hold the values of the next token with size 8.
 * ... and so on.
 *
 * @param depth The number of tokens in the key.
 * @param ... The integer tokens of the key.
 *
 * @return An unsigned integer representation of the key.
 */
unsigned int key_from_tokens(int depth, ...) {
  va_list valist;
  unsigned int key = 0;
  int base = 1;
  va_start(valist, depth);
  for (int i = 0; i < depth; i++) {
    base <<= i;
    key += va_arg(valist, int) * base;
  }
  va_end(valist);
  return key;
}

