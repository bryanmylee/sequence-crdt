#include <stdarg.h>
#include <stdlib.h>
#include "guid.h"

/**
 * @brief Get the number of bits required to store a key with a certain depth.
 *
 * @param depth The depth of the key.
 *
 * @return The number of bits required to store the entire key.
 */
static int n_key_bits(int depth) {
  return (depth * (depth + 1)) / 2;
}

/**
 * @brief Get the number of bytes required to store a given number of bits.
 *
 * This is a fast method to compute math.ceil(x / 8).
 *
 * @param bits The number of bits to store.
 *
 * @return The number of bytes required.
 */
static int n_bytes_for_bits(int bits) {
  return (bits + 7) / 8;
}

static int min(int a, int b) {
  return a < b ? a : b;
}

/**
 * @brief Given keys, set the value of a token at a specified depth only if the
 *        token value at that depth has an initial value of zero.
 *
 * @param keys  The array of bytes representing the keys.
 * @param token The value of the key token to set.
 * @param depth The depth at which to set the token value.
 */
static void keys_set_token(char *keys, unsigned int token, int depth) {
  int base_bit_index = n_key_bits(depth - 1);
  int base_byte_index = base_bit_index / 8;
  int byte_bit_index = base_bit_index % 8;
  int n_bits_in_base_byte = min(8 - byte_bit_index, depth);

  // assuming token 1111111 is placed across two bytes:
  //    base_bit_index = 21
  //    byte_bit_index = 5
  //          |
  //          111 1111 - n_bits_in_base_byte = 3
  // ...|00000000|00000000|
  //     |
  // base_byte_index = 2

  // set the bits for the base byte.
  keys[base_byte_index] |= (bit_n_ones_c(n_bits_in_base_byte) & token) << byte_bit_index;

  int last_bit_index = base_bit_index + depth - 1;
  int last_byte_index = last_bit_index / 8;
  // token is contained within one byte.
  if (last_byte_index == base_byte_index) {
    return;
  }
  // set bytes between base and last byte exclusive.
  token >>= n_bits_in_base_byte;
  for (int i = base_byte_index + 1; i < last_byte_index; i++) {
    keys[i] = bit_n_ones_c(8) & token;
    token >>= 8;
  }
  // set the last byte.
  keys[last_byte_index] = token;
}

static char *vkeys_from_tokens(int depth, va_list valist) {
  // initialize an array of 0s.
  char *keys = calloc(n_bytes_for_bits(n_key_bits(depth)), 1);
  for (int curr_depth = 1; curr_depth <= depth; curr_depth++) {
    unsigned int token = va_arg(valist, int);
    keys_set_token(keys, token, curr_depth);
  }
  return keys;
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
 * @param depth The number of tokens in the key.
 * @param ... The integer tokens of the key.
 *
 * @return An unsigned long representation of the keys.
 */
char *guid_new_keys_from_tokens(int depth, ...) {
  va_list valist;
  va_start(valist, depth);
  char *key = vkeys_from_tokens(depth, valist);
  va_end(valist);
  return key;
}

unsigned int keys_get_token(char *keys, int depth) {
  int base_bit_index = n_key_bits(depth - 1);
  int base_byte_index = base_bit_index / 8;
  int byte_bit_index = base_bit_index % 8;
  int n_bits_in_base_byte = min(8 - byte_bit_index, depth);

  // assuming token 1111111 is placed across two bytes:
  //    base_bit_index = 21
  //    byte_bit_index = 5
  //          |
  //          111 1111 - n_bits_in_base_byte = 3
  // ...|00000000|00000000|
  //     |
  // base_byte_index = 2

  // get bits from the base byte.
  unsigned int token = (keys[base_byte_index] >> byte_bit_index) & bit_n_ones_c(n_bits_in_base_byte);

  int last_bit_index = base_bit_index + depth - 1;
  int last_byte_index = last_bit_index / 8;
  // token is contained within one byte.
  if (last_byte_index == base_byte_index) {
    return token;
  }
  // get bytes between base and last byte exclusive.
  int i;
  int j;
  for (i = 0, j = base_byte_index + 1; j < last_byte_index; i++, j++) {
    token |= (keys[j] & bit_n_ones_c(8)) << (n_bits_in_base_byte + i * 8);
  }
  // get the last byte.
  int n_bits_in_last_byte = (depth - n_bits_in_base_byte - 1) % 8 + 1;
  token |= (keys[j] & bit_n_ones_c(n_bits_in_last_byte)) << (n_bits_in_base_byte + i * 8);
  return token;
}

static unsigned long vuids_from_tokens(int depth, va_list valist) {
  unsigned long uids = 0;
  for (int i = 0; i < depth; i++) {
    // shift by 6 bits per user id.
    unsigned long token = va_arg(valist, int);
    uids |= token << (i * 6);
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
  unsigned long uids = vuids_from_tokens(depth, valist);
  va_end(valist);
  return uids;
}

/**
 * @brief Initialize a Guid.
 *
 * @param g A pointer to the Guid to initialize.
 */
void guid_init(Guid *g) {
  g->keys = NULL;
  g->uids = 0;
  g->depth = 0;
}

/**
 * @brief Initialize and allocate a new Guid to memory.
 *
 * @return A pointer to the allocated Guid.
 */
Guid *guid_new(void) {
  Guid *new = malloc(sizeof(Guid));
  guid_init(new);
  return new;
}

/**
 * @brief Copy the values of one Guid into another.
 *
 * @param g    A pointer to the Guid receiving the copied data.
 * @param from A pointer to the Guid being copied from.
 */
void guid_copy_into(Guid *g, Guid *from) {
  g->depth = from->depth;
  g->keys = from->keys;
  g->uids = from->uids;
}

/**
 * @brief Free an allocated Guid while also cleaning up any internal data.
 *
 * @param g A pointer to a pointer to the allocated Guid.
 */
void guid_free(Guid **g) {
  guid_free_internal(*g);
  free(*g);
  *g = NULL;
}

void guid_free_internal(Guid *g) {
  free(g->keys);
  g->keys = NULL;
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
int guid_compare(Guid *l, Guid *r) {
  int min_depth = l->depth < r->depth ? l->depth : r->depth;
  unsigned long l_uids = l->uids;
  unsigned long r_uids = r->uids;
  // traverse down the key and uids and compare each token.
  for (int i = 1; i <= min_depth; i++) {
    int l_token_key = keys_get_token(l->keys, i);
    int r_token_key = keys_get_token(r->keys, i);
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
    l_uids >>= 6;
    r_uids >>= 6;
  }
  // the keys and uids match. the deeper key is larger.
  return l->depth - r->depth;
}

bool keys_equal(char *l, char *r, int depth) {
  for (int i = 1; i <= depth; i++) {
    if (keys_get_token(l, i) != keys_get_token(r, i)) {
      return false;
    }
  }
  return true;
}

/**
 * @brief Check equality of two Guids.
 *
 * @param l A pointer to the Guid on the left of the comparison.
 * @param r A pointer to the Guid on the right of the comparison.
 *
 * @return If both Guids are equal, returns true.
 */
bool guid_equal(Guid *l, Guid *r) {
  return l->depth == r->depth
    && keys_equal(l->keys, r->keys, l->depth)
    && l->uids == r->uids;
}

/**
 * @brief Modify a Guid to include an additional token in its encoding.
 *
 * @param g A pointer to the Guid.
 * @param t The token to add to the Guid.
 */
void guid_add_token(Guid *g, token t) {
  keys_set_token(g->keys, t.key, g->depth + 1);

  int uids_base = g->depth * 6;
  g->uids += ((unsigned long) t.uid) << uids_base;
  g->depth++;
}

