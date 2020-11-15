#ifndef CRDT_GUID_H
#define CRDT_GUID_H
#include <stdbool.h>
#include "token.h"

typedef struct {
  unsigned long keys; // bits of multiple keys, up to 10 tokens (55 bits).
  unsigned long uids; // bits of multiple user ids, up to 10 tokens (60 bits).
  int depth;          // length of the Guid.
} Guid;

unsigned long keys_from_tokens(int depth, ...);
unsigned long uids_from_tokens(int depth, ...);
void guid_init(Guid* g);
Guid* guid_new(void);
Guid* guid_copy(Guid* o);
int guid_compare(Guid* l, Guid* r);
bool guid_equal(Guid* l, Guid* r);
void guid_add_token(Guid* g, token t);

#endif

