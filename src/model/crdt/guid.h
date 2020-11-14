#ifndef CRDT_GUID_H
#define CRDT_GUID_H
#include <stdbool.h>
#include "token.h"

typedef struct {
  unsigned long keys; // bits of multiple keys, up to 10 tokens (55 bits).
  unsigned long uids; // bits of multiple user ids, up to 10 tokens (60 bits).
  int depth;          // length of the guid.
} guid;

unsigned long keys_from_tokens(int depth, ...);
unsigned long uids_from_tokens(int depth, ...);
int guid_compare(guid* l, guid* r);
bool guid_equal(guid* l, guid* r);
void guid_add_token(guid* l, token t);

#endif

