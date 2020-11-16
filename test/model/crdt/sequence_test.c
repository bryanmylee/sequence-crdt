#include <check.h>
#include <stdlib.h>
#include <model/arraylist.h>
#include <model/crdt/element.h>
#include <model/crdt/guid.h>
#include <model/crdt/sequence.h>

START_TEST(test_seq_token_between) {
  token l = { .key = 4, .uid = 1 };
  token r = { .key = 6, .uid = 1 };
  token expected = { .key = 5, .uid = 1 };
  token result = seq_token_between(&l, &r, 3, 1);
  ck_assert_uint_eq(result.key, expected.key);
  ck_assert_int_eq(result.uid, expected.uid);
} END_TEST

START_TEST(test_seq_token_between_no_space) {
  token l = { .key = 4, .uid = 1 };
  token r = { .key = 5, .uid = 1 };
  token expected = { .key = -1, .uid = -1 };
  token result = seq_token_between(&l, &r, 3, 1);
  ck_assert_uint_eq(result.key, expected.key);
  ck_assert_int_eq(result.uid, expected.uid);
} END_TEST

START_TEST(test_seq_new_guid_between_siblings) {
  Guid l = {
    .depth = 3,
    .keys = keys_from_tokens(3, 1, 2, 1),
    .uids = uids_from_tokens(3, 1, 1, 1),
  };
  Guid r = {
    .depth = 3,
    .keys = keys_from_tokens(3, 1, 2, 3),
    .uids = uids_from_tokens(3, 1, 1, 1),
  };
  Guid expected = {
    .depth = 3,
    .keys = keys_from_tokens(3, 1, 2, 2),
    .uids = uids_from_tokens(3, 1, 1, 2),
  };
  Guid* result = seq_new_guid_between(&l, &r, 2);
  ck_assert_int_eq(result->depth, expected.depth);
  ck_assert_uint_eq(result->keys, expected.keys);
  ck_assert_int_eq(result->uids, expected.uids);
  free(result);
} END_TEST

START_TEST(test_seq_new_guid_between_siblings_no_space) {
  Guid l = {
    .depth = 3,
    .keys = keys_from_tokens(3, 1, 2, 1),
    .uids = uids_from_tokens(3, 1, 1, 1),
  };
  Guid r = {
    .depth = 3,
    .keys = keys_from_tokens(3, 1, 2, 2),
    .uids = uids_from_tokens(3, 1, 1, 1),
  };
  Guid* result = seq_new_guid_between(&l, &r, 2);
  // next Guid is one level deeper.
  Guid expected = {
    .depth = 4,
    .uids = uids_from_tokens(4, 1, 1, 1, 2),
  };
  int base = 1 << 4;
  ck_assert_int_eq(result->depth, expected.depth);
  ck_assert_uint_ge(result->keys, keys_from_tokens(4, 1, 2, 2, 0));
  ck_assert_uint_lt(result->keys, keys_from_tokens(4, 1, 2, 2, base));
  ck_assert_int_eq(result->uids, expected.uids);
  free(result);
} END_TEST

START_TEST(test_seq_new_guid_between_parent_child) {
  Guid l = {
    .depth = 3,
    .keys = keys_from_tokens(3, 0, 1, 2),
    .uids = uids_from_tokens(3, 1, 1, 1),
  };
  Guid r = {
    .depth = 4,
    .keys = keys_from_tokens(4, 0, 1, 2, 2),
    .uids = uids_from_tokens(4, 1, 1, 1, 1),
  };
  Guid* result = seq_new_guid_between(&l, &r, 1);
  ck_assert_int_lt(guid_compare(&l, result), 0);
  ck_assert_int_lt(guid_compare(result, &r), 0);
  free(result);
} END_TEST

START_TEST(test_seq_new_guid_between_uncle_nephew) {
  Guid l = {
    .depth = 3,
    .keys = keys_from_tokens(3, 0, 1, 2),
    .uids = uids_from_tokens(3, 1, 1, 1),
  };
  Guid r = {
    .depth = 4,
    .keys = keys_from_tokens(4, 1, 1, 2, 2),
    .uids = uids_from_tokens(4, 1, 1, 1, 1),
  };
  Guid* result = seq_new_guid_between(&l, &r, 1);
  ck_assert_int_lt(guid_compare(&l, result), 0);
  ck_assert_int_lt(guid_compare(result, &r), 0);
  free(result);
} END_TEST

START_TEST(test_seq_new_guid_between_nephew_uncle) {
  Guid l = {
    .depth = 4,
    .keys = keys_from_tokens(4, 0, 1, 2, 2),
    .uids = uids_from_tokens(4, 1, 1, 1, 1),
  };
  Guid r = {
    .depth = 3,
    .keys = keys_from_tokens(3, 0, 2, 2),
    .uids = uids_from_tokens(3, 1, 1, 1),
  };
  Guid* result = seq_new_guid_between(&l, &r, 1);
  ck_assert_int_lt(guid_compare(&l, result), 0);
  ck_assert_int_lt(guid_compare(result, &r), 0);
  free(result);
} END_TEST

START_TEST(test_seq_new_guid_between_same_key_different_uid) {
  Guid l = {
    .depth = 3,
    .keys = keys_from_tokens(3, 0, 2, 2),
    .uids = uids_from_tokens(3, 1, 1, 1),
  };
  Guid r = {
    .depth = 3,
    .keys = keys_from_tokens(3, 0, 2, 2),
    .uids = uids_from_tokens(3, 1, 2, 1),
  };
  Guid* result = seq_new_guid_between(&l, &r, 1);
  ck_assert_int_lt(guid_compare(&l, result), 0);
  ck_assert_int_lt(guid_compare(result, &r), 0);
  free(result);
} END_TEST

START_TEST(test_seq_index_of) {
  Sequence* s = seq_new();
  // insert 8 Element pointers.
  for (int i = 0; i < 8; i++) {
    Element* e = element_new();
    e->value = malloc(sizeof(int));
    // give all elements a simple incremental guid.
    // at depth 3, all 8 elements can fit under one node.
    e->id = (Guid) {
      .depth = 3,
      .keys = keys_from_tokens(3, 0, 0, i),
    };
    *((int*) e->value) = i;
    al_add(&s->elements, (void*) e);
  }

  Element to_find;
  element_init(&to_find);
  to_find.id = (Guid) {
    .depth = 3,
    .keys = keys_from_tokens(3, 0, 0, 5),
  };
  int index_of_to_find = seq_index_of_element_or_after(s, &to_find);
  ck_assert_int_eq(index_of_to_find, 5);

  seq_free(&s);
} END_TEST

START_TEST(test_seq_index_of_non_existent) {
  Sequence* s = seq_new();
  // insert 8 Element pointers.
  for (int i = 0; i < 8; i++) {
    Element* e = element_new();
    e->value = malloc(sizeof(int));
    // give all elements a simple incremental guid.
    // at depth 3, all 8 elements can fit under one node.
    e->id = (Guid) {
      .depth = 3,
      .keys = keys_from_tokens(3, 0, 0, i),
    };
    *((int*) e->value) = i;
    al_add(&s->elements, (void*) e);
  }

  // should be at the very end of the sequence.
  Element to_find;
  element_init(&to_find);
  to_find.id = (Guid) {
    .depth = 3,
    .keys = keys_from_tokens(3, 1, 0, 0),
  };
  int index_of_to_find = seq_index_of_element_or_after(s, &to_find);
  ck_assert_int_eq(index_of_to_find, 8);

  seq_free(&s);
} END_TEST

Suite* sequence_suite(void) {
  Suite *s;
  TCase *tc_guid;
  TCase *tc_core;

  s = suite_create("sequence_suite");
  tc_guid = tcase_create("guid");
  tc_core = tcase_create("core");

  tcase_add_test(tc_guid, test_seq_token_between);
  tcase_add_test(tc_guid, test_seq_token_between_no_space);
  tcase_add_test(tc_guid, test_seq_new_guid_between_siblings);
  tcase_add_test(tc_guid, test_seq_new_guid_between_siblings_no_space);
  tcase_add_test(tc_guid, test_seq_new_guid_between_parent_child);
  tcase_add_test(tc_guid, test_seq_new_guid_between_uncle_nephew);
  tcase_add_test(tc_guid, test_seq_new_guid_between_nephew_uncle);
  tcase_add_test(tc_guid, test_seq_new_guid_between_same_key_different_uid);
  /* suite_add_tcase(s, tc_guid); */

  tcase_add_test(tc_core, test_seq_index_of);
  tcase_add_test(tc_core, test_seq_index_of_non_existent);
  suite_add_tcase(s, tc_core);

  return s;
}

int main(void) {
  int no_failed = 0;
  Suite *s;
  SRunner *runner;

  s = sequence_suite();
  runner = srunner_create(s);

  srunner_run_all(runner, CK_NORMAL);
  no_failed = srunner_ntests_failed(runner);
  srunner_free(runner);
  return (no_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

