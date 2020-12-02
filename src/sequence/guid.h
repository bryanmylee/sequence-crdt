#ifndef CRDT_GUID_H
#define CRDT_GUID_H
#include <stdbool.h>
#include "bit.h"
#include "token.h"

typedef struct {
  char *keys; // an array of bytes representing the key.
  unsigned long uids; // bits of multiple user ids, up to 10 tokens (60 bits).
  int depth;  // length of the Guid.
} Guid;

char *keys_from_tokens(int depth, ...);
unsigned int token_from_keys(char *keys, int depth);
unsigned long uids_from_tokens(int depth, ...);
void guid_init(Guid *g);
Guid *guid_new(void);
void guid_copy_into(Guid *g, Guid *from);
void guid_free(Guid **g);
int guid_compare(Guid *l, Guid *r);
bool guid_equal(Guid *l, Guid *r);
void guid_add_token(Guid *g, token t);

#endif

