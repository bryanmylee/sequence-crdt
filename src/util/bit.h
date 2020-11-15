#ifndef BIT_H
#define BIT_H
#include <stddef.h>

void sprint_bits(char* str, size_t const size, void const * const ptr);
void print_bits(size_t const size, void const * const ptr);
unsigned char bit_n_ones(int n);

#endif

