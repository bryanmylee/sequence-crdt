#include <check.h>
#include <stdlib.h>

#include <sequence.h>

START_TEST(test_seq_gen_token_between) {
  token l = { .key = 4, .uid = 1 };
  token r = { .key = 6, .uid = 1 };
  token expected = { .key = 5, .uid = 1 };
  token result = seq_gen_token_between(&l, &r, 3, 1);
  ck_assert_uint_eq(result.key, expected.key);
  ck_assert_int_eq(result.uid, expected.uid);
} END_TEST

START_TEST(test_seq_gen_token_between_no_space) {
  token l = { .key = 4, .uid = 1 };
  token r = { .key = 5, .uid = 1 };
  token expected = { .key = -1, .uid = -1 };
  token result = seq_gen_token_between(&l, &r, 3, 1);
  ck_assert_uint_eq(result.key, expected.key);
  ck_assert_int_eq(result.uid, expected.uid);
} END_TEST

START_TEST(test_seq_gen_guid_between_siblings) {
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
  Guid result;
  seq_gen_guid_between(&result, &l, &r, 2);
  ck_assert_int_eq(result.depth, expected.depth);
  ck_assert_uint_eq(result.keys, expected.keys);
  ck_assert_int_eq(result.uids, expected.uids);
} END_TEST

START_TEST(test_seq_gen_guid_between_siblings_no_space) {
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
  Guid result;
  seq_gen_guid_between(&result, &l, &r, 2);
  // next Guid is one level deeper.
  Guid expected = {
    .depth = 4,
    .uids = uids_from_tokens(4, 1, 1, 1, 2),
  };
  int base = 1 << 4;
  ck_assert_int_eq(result.depth, expected.depth);
  ck_assert_uint_ge(result.keys, keys_from_tokens(4, 1, 2, 2, 0));
  ck_assert_uint_lt(result.keys, keys_from_tokens(4, 1, 2, 2, base));
  ck_assert_int_eq(result.uids, expected.uids);
} END_TEST

START_TEST(test_seq_gen_guid_between_siblings_deep) {
  Guid l = {
    .depth = 10,
    // 000000000 1111111100 111100000 00001101 1010000 110101 00011 1110 110 01 0
    .keys = keys_from_tokens(10, 0, 1, 6, 14, 3, 53, 80, 13, 480, 1020),
    .uids = uids_from_tokens(10, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1),
  };
  Guid r = {
    .depth = 10,
    .keys = keys_from_tokens(10, 0, 1, 6, 14, 3, 53, 80, 13, 480, 1022),
    .uids = uids_from_tokens(10, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1),
  };
  Guid expected = {
    .depth = 10,
    .keys = keys_from_tokens(10, 0, 1, 6, 14, 3, 53, 80, 13, 480, 1021),
    .uids = uids_from_tokens(10, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2),
  };
  Guid result;
  seq_gen_guid_between(&result, &l, &r, 2);
  ck_assert_int_eq(result.depth, expected.depth);
  ck_assert_uint_eq(result.keys & bit_n_ones_l(55), expected.keys & bit_n_ones_l(55));
  ck_assert_uint_eq(result.uids & bit_n_ones_l(60), expected.uids & bit_n_ones_l(60));
} END_TEST

START_TEST(test_seq_gen_guid_between_parent_child) {
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
  Guid result;
  seq_gen_guid_between(&result, &l, &r, 1);
  ck_assert_int_lt(guid_compare(&l, &result), 0);
  ck_assert_int_lt(guid_compare(&result, &r), 0);
} END_TEST

START_TEST(test_seq_gen_guid_between_uncle_nephew) {
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
  Guid result;
  seq_gen_guid_between(&result, &l, &r, 1);
  ck_assert_int_lt(guid_compare(&l, &result), 0);
  ck_assert_int_lt(guid_compare(&result, &r), 0);
} END_TEST

START_TEST(test_seq_gen_guid_between_nephew_uncle) {
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
  Guid result;
  seq_gen_guid_between(&result, &l, &r, 1);
  ck_assert_int_lt(guid_compare(&l, &result), 0);
  ck_assert_int_lt(guid_compare(&result, &r), 0);
} END_TEST

START_TEST(test_seq_gen_guid_between_same_key_different_uid) {
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
  Guid result;
  seq_gen_guid_between(&result, &l, &r, 1);
  ck_assert_int_lt(guid_compare(&l, &result), 0);
  ck_assert_int_lt(guid_compare(&result, &r), 0);
} END_TEST

START_TEST(test_seq_gen_guid_invalid) {
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
  // tokens will be generated until an invalid ordering is found.
  // depth 2 contains an invalid ordering.
  Guid expected = {
    .depth = 2,
    .keys = keys_from_tokens(2, 0, 2),
    .uids = uids_from_tokens(2, 1, 2),
  };
  Guid result;
  // flipped left and right.
  seq_gen_guid_between(&result, &r, &l, 1);
  ck_assert_int_eq(expected.depth, result.depth);
  ck_assert_uint_eq(expected.keys, result.keys);
  ck_assert_uint_eq(expected.uids, result.uids);
} END_TEST

Suite *sequence_guid_suite(void) {
  Suite *s;
  TCase *tc_core;

  s = suite_create("check_sequence_guid_gen");
  tc_core = tcase_create("core");

  tcase_add_test(tc_core, test_seq_gen_token_between);
  tcase_add_test(tc_core, test_seq_gen_token_between_no_space);
  tcase_add_test(tc_core, test_seq_gen_guid_between_siblings);
  tcase_add_test(tc_core, test_seq_gen_guid_between_siblings_no_space);
  tcase_add_test(tc_core, test_seq_gen_guid_between_siblings_deep);
  tcase_add_test(tc_core, test_seq_gen_guid_between_parent_child);
  tcase_add_test(tc_core, test_seq_gen_guid_between_uncle_nephew);
  tcase_add_test(tc_core, test_seq_gen_guid_between_nephew_uncle);
  tcase_add_test(tc_core, test_seq_gen_guid_between_same_key_different_uid);
  tcase_add_test(tc_core, test_seq_gen_guid_invalid);
  suite_add_tcase(s, tc_core);

  return s;
}

