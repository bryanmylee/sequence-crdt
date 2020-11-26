#ifndef BIT_H
#define BIT_H
#include <stddef.h>
#include <stdio.h>

void sprint_bits(char* str, size_t const size, void const * const ptr);
void print_bits(size_t const size, void const * const ptr);
unsigned char bit_n_ones_c(int n);
unsigned int bit_n_ones_i(int n);
unsigned long bit_n_ones_l(int n);

#endif

