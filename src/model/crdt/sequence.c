#include <util/bit.h>
#include <model/arraylist.h>
#include "guid.h"
#include "sequence.h"

#define BOUNDARY 10

Element* _new_header() {
  Element* new = element_new();
  new->id.depth = 1;
  new->id.keys = keys_from_tokens(1, 0);
  return new;
}

Element* _new_trailer() {
  Element* new = element_new();
  new->id.depth = 1;
  new->id.keys = keys_from_tokens(1, 1);
  return new;
}

void seq_init(Sequence* s) {
  s->uid = 0;
  s->version = 0;
  al_init(&s->elements);
  Element* header = _new_header();
  Element* trailer = _new_trailer();
  al_add(&s->elements, header);
  al_add(&s->elements, trailer);
}

Sequence* seq_new(void) {
  Sequence* new = malloc(sizeof(Sequence));
  seq_init(new);
  return new;
}

void seq_free(Sequence** s) {
  seq_free_internal(*s);
  free(*s);
  *s = NULL;
}

void seq_free_internal(Sequence* s) {
  al_free_internal_with_cleanup(&s->elements, (void (*)(void *)) &element_free_internal);
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

token seq_gen_token_between(token* l, token* r, int depth, char uid) {
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

void _r_gen_guid_between(Guid* new_guid, Guid* l, int curr_l_depth, Guid* r, int curr_r_depth, char uid) {
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
    token new_token = seq_gen_token_between(&l_token, &r_token, new_depth, uid);
    return guid_add_token(new_guid, new_token);
  }
  if (interval == 1) {
    guid_add_token(new_guid, l_token);
    l->keys >>= curr_l_depth;
    l->uids >>= 6;
    return _r_gen_guid_between(new_guid, l, curr_l_depth + 1, NULL, 0, uid);
  }
  if (interval == 0) {
    guid_add_token(new_guid, l_token);
    if (l_token.uid < r_token.uid) {
      l->keys >>= curr_l_depth;
      l->uids >>= 6;
      return _r_gen_guid_between(new_guid, l, curr_l_depth + 1, NULL, 0, uid);
    }
    if (l_token.uid == r_token.uid) {
      l->keys >>= curr_l_depth;
      l->uids >>= 6;
      r->keys >>= curr_r_depth;
      r->uids >>= 6;
      return _r_gen_guid_between(new_guid, l, curr_l_depth + 1, r, curr_r_depth + 1, uid);
    }
  }
}

void seq_gen_guid_between(Guid* buf, Guid* l, Guid* r, char uid) {
  guid_init(buf);
  Guid l_guid;
  Guid r_guid;
  guid_copy_into(&l_guid, l);
  guid_copy_into(&r_guid, r);
  _r_gen_guid_between(buf, &l_guid, 1, &r_guid, 1, uid);
}

/**
 * @brief Generate a new Guid at the specified internal index and store the
 *        result in a buffer.
 *
 * @param s      A pointer to the Sequence to generate the Guid on.
 * @param buf    A pointer to the Guid buffer to store the result in.
 * @param iindex The internal index at which to generate the Guid.
 */
void seq_gen_guid_at(Sequence* s, Guid* buf, unsigned int iindex) {
  Guid* before = &((Element*) s->elements.data[iindex - 1])->id;
  Guid* after = &((Element*) s->elements.data[iindex])->id;
  seq_gen_guid_between(buf, before, after, s->uid);
}

bool _is_larger_than_max(Sequence* s, Element* target) {
  Element* last = ((Element**) s->elements.data)[s->elements.size - 1];
  return guid_compare(&target->id, &last->id) > 0;
}

/**
 * @brief Find the internal index of a given element, or its supposed position
 *        if it were inserted into the sequence.
 *
 * @param s      A pointer to the Sequence to search.
 * @param target A pointer to the target Element to find.
 *
 * @return The internal index of an element.
 */
unsigned int seq_iindex_of_element_or_after(Sequence* s, Element* target) {
  if (_is_larger_than_max(s, target)) {
    return s->elements.size;
  }
  unsigned int max_i = s->elements.size - 1;
  unsigned int min_i = 0;
  unsigned int i;
  int compare;
  do {
    i = min_i + (max_i - min_i) / 2;
    Element* next = ((Element**) s->elements.data)[i];
    compare = guid_compare(&next->id, &target->id);
    if (compare == 0 || max_i == min_i) {
      return i;
    }
    if (compare < 0) {
      min_i = i + 1;
    } else if (compare > 0) {
      max_i = i;
    }
  } while (max_i - min_i >= 0);
  return i;
}

unsigned int seq_size(Sequence* s) {
  return s->elements.size - 2;
}

Element* seq_get_element(Sequence* s, unsigned int index) {
  if (index < 0 || index > seq_size(s)) {
    return NULL;
  }
  return ((Element**) s->elements.data)[index + 1];
}

Element* seq_insert(Sequence* s, void* to_insert, unsigned int index) {
  if (index < 0 || index > seq_size(s)) {
    return NULL;
  }
  s->version++;
  Element* new = element_new();
  new->value = to_insert;
  // account for header index.
  seq_gen_guid_at(s, &new->id, index + 1);
  al_add_at(&s->elements, new, index + 1);
  return new;
}

Element* seq_delete(Sequence* s, unsigned int index) {
  if (index < 0 || index > seq_size(s)) {
    return NULL;
  }
  s->version++;
  // account for header index.
  return al_remove_at(&s->elements, index + 1);
}

void seq_remote_insert(Sequence* s, Element* to_insert) {
  s->version++;
  unsigned int iindex = seq_iindex_of_element_or_after(s, to_insert);
  Element* e = seq_get_element(s, iindex - 1);
  if (guid_equal(&e->id, &to_insert->id)) {
    return;
  }
  al_add_at(&s->elements, to_insert, iindex);
}

