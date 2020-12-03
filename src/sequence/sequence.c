#include <string.h>
#include "sequence.h"

#define BOUNDARY 10

/**
 * @brief Get the sequence header or trailer markers.
 *
 * @param is_trailer Whether the marker should be a trailer or header marker.
 *
 * @return A sequence marker.
 */
static Element seq_marker(bool is_trailer) {
  Element new;
  element_init(&new);
  new.id.depth = 1;
  new.id.keys = guid_new_keys_from_tokens(1, is_trailer ? 1 : 0);
  return new;
}

/**
 * @brief Initialize a Sequence.
 *
 * @param s A pointer to the Sequence to initialize.
 */
void seq_init(Sequence *s) {
  s->uid = 0;
  s->version = 0;
  al_init(&s->elements, sizeof(Element));
  Element header = seq_marker(false);
  Element trailer = seq_marker(true);
  al_add(&s->elements, &header);
  al_add(&s->elements, &trailer);
}

/**
 * @brief Initialize and allocate a new Sequence to memory.
 *
 * @return A pointr to the allocated Sequence.
 */
Sequence *seq_new(void) {
  Sequence *new = malloc(sizeof(Sequence));
  seq_init(new);
  return new;
}

/**
 * @brief Free an allocated Sequence while also cleaning up any internal data.
 *
 * @param s A pointer to a pointer to the allocated ArrayList.
 */
void seq_free(Sequence **s) {
  seq_free_internal(*s);
  free(*s);
  *s = NULL;
}

/**
 * @brief Free up any internal data of the Sequence.
 *
 * @param s A pointer to the Sequence.
 */
void seq_free_internal(Sequence *s) {
  al_free_internal_cleanup(&s->elements, (void (*)(void *e)) element_free_internal);
}

/**
 * @brief Get the boundary strategy at the given depth as defined by LSEQ.
 *
 * @param depth The given depth.
 *
 * @return The boundary strategy. Returns true if the strategy is boundary+.
 *         Otherwise, returns false if the strategy is boundary-.
 */
static bool is_boundary_plus(int depth) {
  return depth % 2 == 0;
}

/**
 * @brief Get the number of potential nodes at a given depth.
 *
 * @param depth The given depth.
 *
 * @return The number of potential nodes.
 */
static int get_base(int depth) {
  return 1 << depth;
}

/**
 * @brief Generate a token between two tokens with at least one empty space
 *        between using LSEQ.
 *
 * The token is set at a random offset from the left or right token, depending
 * on the boundary strategy at the given depth.
 *
 * @param l     A pointer to the token on the left.
 * @param r     A pointer to the token on the right.
 * @param depth The depth at which the token is being generated.
 * @param uid   The uid of the sequence generating the token.
 *
 * @return The generated token.
 */
token seq_gen_token_between(token *l, token *r, int depth, char uid) {
  int interval = r->key - l->key - 1;
  if (interval < 1) {
    // invalid token.
    return (token) { .key = -1, .uid = -1 };
  }
  int step = BOUNDARY < interval ? BOUNDARY : interval;
  int rand_step = (rand() % step) + 1;
  if (is_boundary_plus(depth)) {
    return (token) { .key = l->key + rand_step, .uid = uid };
  }
  return (token) { .key = r->key - rand_step, .uid = uid };
}

static void r_gen_guid_between(Guid *new_guid, Guid *l, int curr_l_depth, Guid *r, int curr_r_depth, char uid) {
  int new_depth = new_guid->depth + 1;

  bool l_has_next = curr_l_depth <= l->depth;
  token l_token = {
    .key = l_has_next ? keys_get_token(l->keys, curr_l_depth) : 0,
    .uid = l_has_next ? l->uids & 63 : uid,
  };

  bool r_has_next = r != NULL && curr_r_depth <= r->depth;
  token r_token = {
    .key = r_has_next ? keys_get_token(r->keys, curr_r_depth): get_base(new_depth),
    .uid = r_has_next ? r->uids & 63 : uid,
  };

  int interval = r_token.key - l_token.key;
  if (interval > 1) {
    token new_token = seq_gen_token_between(&l_token, &r_token, new_depth, uid);
    guid_add_token(new_guid, new_token);
    return;
  }
  if (interval == 1) {
    guid_add_token(new_guid, l_token);
    l->uids >>= 6;
    return r_gen_guid_between(new_guid, l, curr_l_depth + 1, NULL, 0, uid);
  }
  if (interval == 0) {
    guid_add_token(new_guid, l_token);
    if (l_token.uid < r_token.uid) {
      l->uids >>= 6;
      return r_gen_guid_between(new_guid, l, curr_l_depth + 1, NULL, 0, uid);
    }
    if (l_token.uid == r_token.uid) {
      l->uids >>= 6;
      r->uids >>= 6;
      return r_gen_guid_between(new_guid, l, curr_l_depth + 1, r, curr_r_depth + 1, uid);
    }
  }
  // If this line is reached, there is a serious error with the order of Guids.
}

void seq_gen_guid_between(Guid *buf, Guid *l, Guid *r, char uid) {
  guid_init(buf);
  // TODO reduce size of key if not used.
  // give the new guid's key an extra depth just in case.
  int max_depth = l->depth > r->depth ? l->depth : r->depth;
  buf->keys = calloc(max_depth + 1, 1);
  Guid l_guid;
  Guid r_guid;
  guid_copy(&l_guid, l);
  guid_copy(&r_guid, r);
  r_gen_guid_between(buf, &l_guid, 1, &r_guid, 1, uid);
}

/**
 * @brief Generate a new Guid at the specified internal index and store the
 *        result in a buffer.
 *
 * @param s      A pointer to the Sequence to generate the Guid on.
 * @param buf    A pointer to the Guid buffer to store the result in.
 * @param iindex The internal index at which to generate the Guid.
 */
void seq_gen_guid_at(Sequence *s, Guid *buf, unsigned int iindex) {
  Guid *before = &((Element *) al_get(&s->elements, iindex - 1))->id;
  Guid *after = &((Element *) al_get(&s->elements, iindex))->id;
  seq_gen_guid_between(buf, before, after, s->uid);
}

static bool is_larger_than_max(Sequence *s, Element *target) {
  Element *last = al_get(&s->elements, s->elements.size - 1);
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
unsigned int seq_iindex_of_element_or_after(Sequence *s, Element *target) {
  if (is_larger_than_max(s, target)) {
    return s->elements.size;
  }
  unsigned int max_i = s->elements.size - 1;
  unsigned int min_i = 0;
  unsigned int i;
  int compare;
  do {
    i = min_i + (max_i - min_i) / 2;
    Element *next = al_get(&s->elements, i);
    compare = guid_compare(&next->id, &target->id);
    if (compare == 0 || max_i == min_i) {
      break;
    }
    if (compare < 0) {
      min_i = i + 1;
    } else if (compare > 0) {
      max_i = i;
    }
  } while (max_i >= min_i);
  return i;
}

unsigned int seq_size(Sequence *s) {
  return s->elements.size - 2;
}

/**
 * @brief Get the element in the sequence with a specified index.
 *
 * This method treats the sequence as an ordered collection of elements, not
 * inclusive of the header and trailer elements.
 *
 * @param s     The sequence to get the element from.
 * @param index The index of the element.
 *
 * @return A pointer to the element in the sequence.
 */
Element *seq_get_element(Sequence *s, unsigned int index) {
  if (index < 0 || index >= seq_size(s)) {
    return NULL;
  }
  return al_get(&s->elements, index + 1);
}

static bool i_seq_insert(Sequence *s, void *insert_ptr, size_t esize, long insert_val, unsigned int index, Element *buf) {
  if (index < 0 || index > seq_size(s)) {
    return false;
  }
  s->version++;
  Element new = { .version = s->version };
  if (insert_ptr == NULL) {
    element_set_value(&new, insert_val);
  } else {
    element_set_ptr(&new, insert_ptr, esize);
  }
  // account for header index.
  seq_gen_guid_at(s, &new.id, index + 1);
  al_add_at(&s->elements, &new, index + 1);
  if (buf != NULL) {
    memcpy(buf, &new, sizeof(Element));
  }
  return true;
}

bool seq_insert(Sequence *s, void *to_insert, size_t esize, unsigned int index) {
  return i_seq_insert(s, to_insert, esize, 0, index, NULL);
}

bool seq_insert_save(Sequence *s, void *to_insert, size_t esize, unsigned int index, Element *buf) {
  return i_seq_insert(s, to_insert, esize, 0, index, buf);
}

bool seq_insert_value(Sequence *s, long to_insert, unsigned int index) {
  return i_seq_insert(s, NULL, 0, to_insert, index, NULL);
}

bool seq_insert_value_save(Sequence *s, long to_insert, unsigned int index, Element *buf) {
  return i_seq_insert(s, NULL, 0, to_insert, index, buf);
}

static bool i_seq_delete(Sequence *s, unsigned int index, Element *buf) {
  if (index < 0 || index >= seq_size(s)) {
    return false;
  }
  s->version++;
  // account for header index.
  if (buf != NULL) {
    al_remove_at_save(&s->elements, index + 1, buf);
  } else {
    al_remove_at_cleanup(&s->elements, index + 1, (void (*)(void *e)) element_free_internal);
  }
  return true;
}

/**
 * @brief Delete an element from the Sequence.
 *
 * The deleted element is automatically freed.
 *
 * @param s     The Sequence to delete the Element from.
 * @param index The index of the Element to delete.
 *
 * @return True if the Element was successfully deleted, otherwise false.
 */
bool seq_delete(Sequence *s, unsigned int index) {
  return i_seq_delete(s, index, NULL);
}

/**
 * @brief Delete an element from the Sequence and save it into a buffer.
 *
 * Saved elements are not automatically freed when the sequence is freed, and
 * must be freed by the caller.
 *
 * @param s     The Sequence to delete the Element from.
 * @param index The index of the Element to delete.
 * @param buf   The buffer to store the Element in after deletion.
 *
 * @return True if the Element was successfully deleted, otherwise false.
 */
bool seq_delete_save(Sequence *s, unsigned int index, Element *buf) {
  return i_seq_delete(s, index, buf);
}

/**
 * @brief Insert a remotely generated Element into the Sequence.
 *
 * The Element being inserted must not already belong to another Sequence
 * structure.
 *
 * @param s         The Sequence to insert into.
 * @param to_insert The remotely generated Element.
 *
 * @return True if the element was inserted successfully, otherwise false.
 */
bool seq_remote_insert(Sequence *s, Element *to_insert) {
  unsigned int iindex = seq_iindex_of_element_or_after(s, to_insert);
  Element *e = al_get(&s->elements, iindex);
  if (guid_equal(&e->id, &to_insert->id)) {
    return false;
  }
  s->version++;
  al_add_at(&s->elements, to_insert, iindex);
  return true;
}

bool seq_remote_delete(Sequence *s, Element *to_delete) {
  unsigned int iindex = seq_iindex_of_element_or_after(s, to_delete);
  Element *e = al_get(&s->elements, iindex);
  if (!guid_equal(&e->id, &to_delete->id)) {
    return false;
  }
  s->version++;
  al_remove_at(&s->elements, iindex);
  return true;
}

void seq_gen_chars(Sequence *s, char *buf) {
  unsigned int n = seq_size(s);
  for (unsigned int i = 0; i < n; i++) {
    buf[i] = seq_get_element(s, i)->data.value;
  }
  buf[n] = 0;
}

