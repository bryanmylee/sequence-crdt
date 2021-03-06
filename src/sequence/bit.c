#include "bit.h"

/**
 * @brief Compose a string with the same text that would be printed by print_bits.
 *
 * @param str A pointer to a string buffer where the result is stored.
 * @param size The size of the object.
 * @param ptr A pointer to the object.
 */
void sprint_bits(char *str, size_t const size, void const * const ptr) {
  unsigned char *b = (unsigned char *) ptr;
  unsigned char byte;
  int k = 0;
  for (int i = size - 1; i >= 0; i--) {
    for (int j = 7; j >= 0; j--) {
      byte = (b[i] >> j) & 1;
      sprintf(str + k++, "%u", byte);
    }
  }
}

/**
 * @brief Print out a bit representation of the object being pointed to in
 * little-endian.
 *
 * @param size The size of the object.
 * @param ptr A pointer to the object.
 */
void print_bits(size_t const size, void const * const ptr) {
  char str[size * 8 + 1];
  sprint_bits(str, size, ptr);
  printf("%s\n", str);
}

unsigned char bit_n_ones_c(int n) {
  return (1 << n) - 1;
}

/**
 * @brief Generate bitmasks of up to 32 bits in length.
 *
 * @param n The length of the bitmask.
 *
 * @return The bitmask.
 */
unsigned int bit_n_ones_i(int n) {
  return n >= 32 ? 0xffffffffUL : (1 << n) - 1;
}


/**
 * @brief Generate bitmasks of up to 64 bits in length.
 *
 * @param n The length of the bitmask.
 *
 * @return The bitmask.
 */
unsigned long bit_n_ones_l(int n) {
  return n >= 64 ? 0xffffffffffffffffUL : (1UL << n) - 1;
}

