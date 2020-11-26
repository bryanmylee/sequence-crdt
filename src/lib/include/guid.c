#include <stdarg.h>
#include <stdlib.h>
#include "../guid.h"

unsigned long _vkeys_from_tokens(int depth, va_list valist) {
  unsigned long key = 0;
  int base = 0;
  for (int i = 0; i < depth; i++) {
    key += va_arg(valist, unsigned long) << base;
    base += i + 1;
  }
  return key;
}

/**
 * @brief Get the encoded data for a given list of keys.
 *
 * Given a key comprising k tokens, each token doubling in base size
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
 * @return An unsigned long representation of the keys.
 */
unsigned long keys_from_tokens(int depth, ...) {
  va_list valist;
  va_start(valist, depth);
  unsigned long key = _vkeys_from_tokens(depth, valist);
  va_end(valist);
  return key;
}

unsigned long _vuids_from_tokens(int depth, va_list valist) {
  unsigned long uids = 0;
  for (int i = 0; i < depth; i++) {
    // shift by 6 bits per user id.
    uids += va_arg(valist, unsigned long) << (i * 6);
  }
  return uids;
}

/**
 * @brief Get the encoded data for a given list of uids.
 *
 * Given a key comprising k tokens, we need to store k attached user ids.
 * To match the key tokens and fit 10 user ids in an unsigned long (64 bits),
 * we can use 6-bit user ids for a total size of 6 * 10 = 60 bits < 64 bits.
 *
 * This means that we can have up to 2^6 = 64 concurrent users using the sequence.
 *
 * @param depth The number of tokens in the key.
 * @param ... The integer tokens of the key.
 *
 * @return An unsigned long representation of the uids.
 */
unsigned long uids_from_tokens(int depth, ...) {
  va_list valist;
  va_start(valist, depth);
  unsigned long uids = _vuids_from_tokens(depth, valist);
  va_end(valist);
  return uids;
}

/**
 * @brief Initialize a Guid.
 *
 * @param g A pointer to the Guid to initialize.
 */
void guid_init(Guid* g) {
  g->keys = 0;
  g->uids = 0;
  g->depth = 0;
}

/**
 * @brief Initialize and allocate a new Guid to memory.
 *
 * @return A pointer to the allocated Guid.
 */
Guid* guid_new(void) {
  Guid* new = malloc(sizeof(Guid));
  guid_init(new);
  return new;
}

/**
 * @brief Copy the values of one Guid into another.
 *
 * @param g    A pointer to the Guid receiving the copied data.
 * @param from A pointer to the Guid being copied from.
 */
void guid_copy_into(Guid* g, Guid* from) {
  g->depth = from->depth;
  g->keys = from->keys;
  g->uids = from->uids;
}

/**
 * @brief Free an allocated Guid while also cleaning up any internal data.
 *
 * @param g A pointer to a pointer to the allocated Guid.
 */
void guid_free(Guid** g) {
  free(*g);
  *g = NULL;
}

/**
 * @brief Compare the order of two Guids.
 *
 * Order is first determined by each token in the keys. If the tokens match,
 * order is then determined by the token in the uids.
 *
 * If two Guids have the same keys and uids, then order is solely dependent on
 * the depth of the Guid.
 *
 * @param l A pointer to the Guid on the left of the comparison.
 * @param r A pointer to the Guid on the right of the comparison.
 *
 * @return The order of the Guids.
 *         - If l < r, returns a negative number.
 *         - If l > r, returns a positive number.
 *         - Otherwise, if l == r, returns 0.
 */
int guid_compare(Guid* l, Guid* r) {
  int min_depth = l->depth < r->depth ? l->depth : r->depth;
  unsigned long l_keys = l->keys;
  unsigned long r_keys = r->keys;
  unsigned long l_uids = l->uids;
  unsigned long r_uids = r->uids;
  // traverse down the key and uids and compare each token.
  for (int i = 1; i <= min_depth; i++) {
    int l_token_key = l_keys & bit_n_ones_i(i);
    int r_token_key = r_keys & bit_n_ones_i(i);
    int kcompare = l_token_key - r_token_key;
    if (kcompare != 0) {
      return kcompare;
    }
    int l_token_uid = l_uids & 63;
    int r_token_uid = r_uids & 63;
    int ucompare = l_token_uid - r_token_uid;
    if (ucompare != 0) {
      return ucompare;
    }
    l_keys >>= i;
    r_keys >>= i;
    l_uids >>= 6;
    r_uids >>= 6;
  }
  // the keys and uids match. the deeper key is larger.
  return l->depth - r->depth;
}

/**
 * @brief Check equality of two Guids.
 *
 * @param l A pointer to the Guid on the left of the comparison.
 * @param r A pointer to the Guid on the right of the comparison.
 *
 * @return If both Guids are equal, returns true.
 */
bool guid_equal(Guid* l, Guid* r) {
  return l->depth == r->depth && l->keys == r->keys && l->uids == r->uids;
}

/**
 * @brief Modify a Guid to include an additional token in its encoding.
 *
 * @param g A pointer to the Guid.
 * @param t The token to add to the Guid.
 */
void guid_add_token(Guid* g, token t) {
  int key_base = g->depth * (g->depth + 1) / 2;
  g->keys += ((unsigned long) t.key) << key_base;
  int uids_base = g->depth * 6;
  g->uids += ((unsigned long) t.uid) << uids_base;
  g->depth++;
}

