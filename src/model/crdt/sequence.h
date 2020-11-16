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
  ArrayList elements;
} Sequence;

void seq_init(Sequence* s);
Sequence* seq_new(void);
void seq_free(Sequence** s);
void seq_free_internal(Sequence* s);
token seq_token_between(token* l, token* r, int depth, char uid);
void seq_guid_between(Guid* buf, Guid* l, Guid* r, char uid);
void seq_guid_at(Guid* buf, Sequence* s, unsigned int index);
unsigned int seq_index_of_element_or_after(Sequence* s, Element* e);

#endif

