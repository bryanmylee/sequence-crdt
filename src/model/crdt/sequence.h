#ifndef CRDT_SEQUENCE_H
#define CRDT_SEQUENCE_H
#include <stdbool.h>
#include <model/arraylist.h>
#include "element.h"
#include "guid.h"
#include "token.h"

typedef struct {
  char uid;
  unsigned int version;
  ArrayList elements;   // an ArrayList of Element structs.
} Sequence;

void seq_init(Sequence* s);
Sequence* seq_new(void);
void seq_free(Sequence** s);
void seq_free_internal(Sequence* s);
token seq_gen_token_between(token* l, token* r, int depth, char uid);
void seq_gen_guid_between(Guid* buf, Guid* l, Guid* r, char uid);
void seq_gen_guid_at(Sequence* s, Guid* buf, unsigned int index);
unsigned int seq_iindex_of_element_or_after(Sequence* s, Element* e);
unsigned int seq_size(Sequence* s);
Element* seq_get_element(Sequence* s, unsigned int index);
bool seq_insert(Sequence* s, void* to_insert, unsigned int index);
bool seq_insert_save(Sequence* s, void* to_insert, unsigned int index, Element* buf);
bool seq_insert_value(Sequence* s, char to_insert, unsigned int index);
bool seq_insert_value_save(Sequence* s, char to_insert, unsigned int index, Element* buf);
bool seq_delete(Sequence* s, unsigned int index);
bool seq_delete_save(Sequence* s, unsigned int index, Element* buf);
bool seq_remote_insert(Sequence* s, Element* to_insert);
bool seq_remote_delete(Sequence* s, Element* to_delete);

#endif

