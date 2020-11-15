#include <check.h>
#include <stdlib.h>
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

Suite* element_suite(void) {
  Suite *s;
  TCase *tc_guid;

  s = suite_create("sequence_suite");
  tc_guid = tcase_create("guid");

  tcase_add_test(tc_guid, test_seq_token_between);
  tcase_add_test(tc_guid, test_seq_token_between_no_space);
  tcase_add_test(tc_guid, test_seq_new_guid_between_siblings);
  tcase_add_test(tc_guid, test_seq_new_guid_between_siblings_no_space);
  tcase_add_test(tc_guid, test_seq_new_guid_between_parent_child);
  tcase_add_test(tc_guid, test_seq_new_guid_between_uncle_nephew);
  tcase_add_test(tc_guid, test_seq_new_guid_between_nephew_uncle);
  tcase_add_test(tc_guid, test_seq_new_guid_between_same_key_different_uid);
  suite_add_tcase(s, tc_guid);

  return s;
}

int main(void) {
  int no_failed = 0;
  Suite *s;
  SRunner *runner;

  s = element_suite();
  runner = srunner_create(s);

  srunner_run_all(runner, CK_NORMAL);
  no_failed = srunner_ntests_failed(runner);
  srunner_free(runner);
  return (no_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

