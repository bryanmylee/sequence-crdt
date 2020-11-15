#ifndef CRDT_SEQUENCE_H
#define CRDT_SEQUENCE_H
#include <stdbool.h>
#include <model/arraylist.h>
#include "guid.h"
#include "token.h"

typedef struct {
  char uid;
  unsigned int version;
  arraylist elements;
} sequence;

token get_token_between(token* l, token* r, int depth, char uid);
guid get_guid_between(guid* l, guid* r, char uid);
guid seq_new_guid_at(sequence* s, unsigned int index);

#endif

