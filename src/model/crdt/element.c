#include <stdio.h>
#include "element.h"

unsigned long vkey_from_tokens(int depth, va_list valist) {
  unsigned long key = 0;
  int base = 1;
  for (int i = 0; i < depth; i++) {
    base <<= i;
    key += va_arg(valist, unsigned long) * base;
  }
  return key;
}

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
 * With an unsigned long (64 bits), a total of 10 tokens can be fitted:
 * 1 + 2 + 3 + ... + 10 = 55 bits < 64 bits.
 *
 * @param depth The number of tokens in the key.
 * @param ... The integer tokens of the key.
 *
 * @return An unsigned integer representation of the key.
 */
unsigned long key_from_tokens(int depth, ...) {
  va_list valist;
  va_start(valist, depth);
  unsigned long key = vkey_from_tokens(depth, valist);
  va_end(valist);
  return key;
}

unsigned long vuids_from_tokens(int depth, va_list valist) {
  unsigned long uids = 0;
  for (int i = 0; i < depth; i++) {
    // shift by 6 bits per user id.
    uids += va_arg(valist, unsigned long) << (i * 6);
  }
  return uids;
}

/**
 * @brief Given a key comprising k tokens, we need to store k attached user ids.
 * To match the key tokens and fit 10 user ids in an unsigned long (64 bits),
 * we can use 6-bit user ids for a total size of 6 * 10 = 60 bits < 64 bits.
 *
 * @param depth The number of tokens in the key.
 * @param ... The integer tokens of the key.
 *
 * @return An unsigned integer representation of the key.
 */
unsigned long uids_from_tokens(int depth, ...) {
  va_list valist;
  va_start(valist, depth);
  unsigned long uids = vuids_from_tokens(depth, valist);
  va_end(valist);
  return uids;
}

int key_compare(element* l, element* r) {
  int min_depth = l->depth < r->depth ? l->depth : r->depth;
  unsigned long l_key = l->key;
  unsigned long r_key = r->key;
  unsigned long l_uids = l->uids;
  unsigned long r_uids = r->uids;
  int base = 2;
  // traverse down the key and uids and compare each token.
  for (int i = 1; i <= min_depth; i++) {
    int l_key_token = l_key % base;
    int r_key_token = r_key % base;
    int kcompare = l_key_token - r_key_token;
    if (kcompare != 0) {
      return kcompare;
    }
    int l_uids_token = l_uids & 63;
    int r_uids_token = r_uids & 63;
    int ucompare = l_uids_token - r_uids_token;
    if (ucompare != 0) {
      return ucompare;
    }
    l_key /= base;
    r_key /= base;
    base <<= i;
    l_uids >>= 6;
    r_uids >>= 6;
  }
  // the keys and uids match. the deeper key is larger.
  return l->depth - r->depth;
}

bool key_equal(element* l, element* r) {
  return l->depth == r->depth && l->key == r->key && l->uids == r->uids;
}

void vset_key_tokens(element* e, int depth, va_list valist) {
  e->key = vkey_from_tokens(depth, valist);
  e->depth = depth;
}

void set_key_tokens(element* e, int depth, ...) {
  va_list valist;
  va_start(valist, depth);
  vset_key_tokens(e, depth, valist);
  va_end(valist);
}

