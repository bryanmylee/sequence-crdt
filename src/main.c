#include <stdio.h>
#include <sequence.h>

int main(void) {
  Sequence* s = seq_new();
  char* data = "the sequence does not leak memory!";
  int n = strlen(data);
  for (int i = 0; i < n; i++) {
    seq_insert_value(s, data[i], i);
  }
  char output[100];
  seq_gen_chars(s, output);
  printf("%s\n", output);
  seq_free(&s);
  return 0;
}

