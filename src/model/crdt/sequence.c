#include <util/bit.h>
#include "guid.h"
#include "sequence.h"

#define BOUNDARY 10

void seq_init(Sequence* s) {
  s->uid = 0;
  s->version = 0;
  al_init(&s->elements);
}

Sequence* seq_new(void) {
  Sequence* new = malloc(sizeof(Sequence));
  seq_init(new);
  return new;
}

/**
 * @brief Get the boundary strategy at the given depth as defined by LSEQ.
 *
 * @param depth The given depth.
 *
 * @return The boundary strategy. Returns true if the strategy is boundary+.
 *         Otherwise, returns false if the strategy is boundary-.
 */
bool _is_boundary_plus(int depth) {
  return depth % 2 == 0;
}

int _get_base(int depth) {
  return 1 << depth;
}

token seq_token_between(token* l, token* r, int depth, char uid) {
  int interval = r->key - l->key - 1;
  if (interval < 1) {
    // invalid token.
    return (token) { .key = -1, .uid = -1 };
  }
  int step = BOUNDARY < interval ? BOUNDARY : interval;
  int rand_step = (rand() % step) + 1;
  if (_is_boundary_plus(depth)) {
    return (token) { .key = l->key + rand_step, .uid = uid };
  }
  return (token) { .key = r->key - rand_step, .uid = uid };
}

void _r_generate_guid_between(Guid* new_guid, Guid* l, int curr_l_depth, Guid* r, int curr_r_depth, char uid) {
  int new_depth = new_guid->depth + 1;

  bool l_has_next = curr_l_depth <= l->depth;
  token l_token = {
    .key = l_has_next ? l->keys & bit_n_ones(curr_l_depth) : 0,
    .uid = l_has_next ? l->uids & 63 : uid,
  };

  bool r_has_next = r != NULL && curr_r_depth <= r->depth;
  token r_token = {
    .key = r_has_next ? r->keys & bit_n_ones(curr_r_depth) : _get_base(new_depth),
    .uid = r_has_next ? r->uids & 63 : uid,
  };

  int interval = r_token.key - l_token.key;
  if (interval > 1) {
    token new_token = seq_token_between(&l_token, &r_token, new_depth, uid);
    return guid_add_token(new_guid, new_token);
  }
  if (interval == 1) {
    guid_add_token(new_guid, l_token);
    l->keys >>= curr_l_depth;
    l->uids >>= 6;
    return _r_generate_guid_between(new_guid, l, curr_l_depth + 1, NULL, 0, uid);
  }
  if (interval == 0) {
    guid_add_token(new_guid, l_token);
    if (l_token.uid < r_token.uid) {
      l->keys >>= curr_l_depth;
      l->uids >>= 6;
      return _r_generate_guid_between(new_guid, l, curr_l_depth + 1, NULL, 0, uid);
    }
    if (l_token.uid == r_token.uid) {
      l->keys >>= curr_l_depth;
      l->uids >>= 6;
      r->keys >>= curr_r_depth;
      r->uids >>= 6;
      return _r_generate_guid_between(new_guid, l, curr_l_depth + 1, r, curr_r_depth + 1, uid);
    }
  }
}

Guid* seq_new_guid_between(Guid* l, Guid* r, char uid) {
  Guid* l_guid = guid_copy(l);
  Guid* r_guid = guid_copy(r);
  Guid* new_guid = guid_new();
  _r_generate_guid_between(new_guid, l_guid, 1, r_guid, 1, uid);
  free(l_guid);
  free(r_guid);
  return new_guid;
}

Guid* seq_new_guid_at(Sequence* s, unsigned int index) {
  Guid* before = &((Element*) s->elements.data[index - 1])->id;
  Guid* after = &((Element*) s->elements.data[index])->id;
  return seq_new_guid_between(before, after, s->uid);
}

unsigned int seq_index_of_element_or_after(Sequence* s, Element* e) {
  int i = 0;
  while (i < s->elements.size
      && guid_compare(&e->id, &((Element**) s->elements.data)[i]->id) > 0) {
    i++;
  }
  return i;
}

