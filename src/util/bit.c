#include <stddef.h>
#include <stdio.h>

/**
 * @brief Print out a bit representation of the object being pointed to in
 * little-endian.
 *
 * @param size The size of the object.
 * @param ptr A pointer to the object.
 */
void print_bits(size_t const size, void const * const ptr) {
  unsigned char *b = (unsigned char*) ptr;
  unsigned char byte;
  for (int i = size - 1; i >= 0; i--) {
    for (int j = 7; j >= 0; j--) {
      byte = (b[i] >> j) & 1;
      printf("%u", byte);
    }
  }
  puts("");
}

