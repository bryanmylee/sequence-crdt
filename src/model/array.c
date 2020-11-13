#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>

#define INITIAL_CAPACITY 0

typedef struct {
  int capacity;
  int n;
  int* items;
} darray;

static bool resize(darray* array, int capacity);

darray* new(void);

int size(const darray* array);

