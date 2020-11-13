#include <check.h>
#include <src/model/crdt/element.h>

START_TEST(test_key_from_tokens) {
  int result = key_from_tokens(3, 0, 2, 7);
  ck_assert_uint_eq(60, result);
} END_TEST

