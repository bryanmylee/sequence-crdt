#include <check.h>
#include <stdlib.h>

#include <guid.h>

START_TEST(test_guid_init) {
  Guid g;
  guid_init(&g);
  ck_assert_ptr_null(g.keys);
  ck_assert_int_eq(g.uids, 0);
  ck_assert_int_eq(g.depth, 0);
} END_TEST

START_TEST(test_guid_new) {
  Guid *g = guid_new();
  ck_assert_ptr_null(g->keys);
  ck_assert_int_eq(g->uids, 0);
  ck_assert_int_eq(g->depth, 0);
  guid_free(&g);
} END_TEST

START_TEST(test_guid_copy_into) {
  // depreciated function.
} END_TEST

START_TEST(test_guid_free) {
  Guid *g = guid_new();
  char *keys = g->keys;
  guid_free(&g);
  ck_assert_ptr_null(g);
  ck_assert_ptr_null(keys);
} END_TEST

START_TEST(test_keys_from_tokens) {
  char *result = guid_new_keys_from_tokens(3, 0, 2, 7);
  ck_assert_uint_eq(*result, 60);
  free(result);

  result = guid_new_keys_from_tokens(3, 0, 1, 2);
  ck_assert_uint_eq(*result, 18);
  free(result);
} END_TEST

START_TEST(test_keys_from_tokens_deep) {
  char *result = guid_new_keys_from_tokens(10, 0, 2, 7, 1, 30, 4, 118, 40, 506, 1018);
  ck_assert_uint_eq(*(unsigned long *) result & bit_n_ones_l(55), 0b1111111010111111010001010001110110000100111100001111100 & bit_n_ones_l(55));

  free(result);
} END_TEST


START_TEST(test_uids_from_tokens) {
  unsigned long result;
  result = uids_from_tokens(3, 1, 1, 2);
  ck_assert_uint_eq(result, 8257);
  result = uids_from_tokens(4, 1, 1, 1, 1);
  ck_assert_uint_eq(result, 266305);
} END_TEST

START_TEST(test_guid_add_token) {
  Guid g = {
    .depth = 3,
    .keys = guid_new_keys_from_tokens(3, 0, 0, 0),
    .uids = uids_from_tokens(3, 1, 1, 1),
  };
  token t = { .key = 1, .uid = 1 };
  guid_add_token(&g, t);

  Guid expected = {
    .depth = 4,
    .keys = guid_new_keys_from_tokens(4, 0, 0, 0, 1),
    .uids = uids_from_tokens(4, 1, 1, 1, 1),
  };
  ck_assert_int_eq(g.depth, expected.depth);
  ck_assert_int_eq(*g.keys, *expected.keys);
  ck_assert_int_eq(g.uids, expected.uids);

  guid_free_internal(&g);
  guid_free_internal(&expected);
} END_TEST

START_TEST(test_guid_add_token_second_uid) {
  Guid g = {
    .depth = 3,
    .keys = guid_new_keys_from_tokens(3, 0, 0, 0),
    .uids = uids_from_tokens(3, 1, 1, 1),
  };
  token t = { .key = 1, .uid = 2 };
  guid_add_token(&g, t);

  Guid expected = {
    .depth = 4,
    .keys = guid_new_keys_from_tokens(4, 0, 0, 0, 1),
    .uids = uids_from_tokens(4, 1, 1, 1, 2),
  };
  ck_assert_int_eq(g.depth, expected.depth);
  ck_assert_int_eq(*g.keys, *expected.keys);
  ck_assert_int_eq(g.uids, expected.uids);

  guid_free_internal(&g);
  guid_free_internal(&expected);
} END_TEST

Suite *guid_gen_suite(void) {
  Suite *s;
  TCase *tc_core;

  s = suite_create("check_guid_gen");
  tc_core = tcase_create("core");

  // Creation of key test case
  tcase_add_test(tc_core, test_guid_init);
  tcase_add_test(tc_core, test_guid_new);
  tcase_add_test(tc_core, test_guid_copy_into);
  tcase_add_test(tc_core, test_guid_free);
  tcase_add_test(tc_core, test_keys_from_tokens);
  tcase_add_test(tc_core, test_keys_from_tokens_deep);
  tcase_add_test(tc_core, test_uids_from_tokens);
  tcase_add_test(tc_core, test_guid_add_token);
  tcase_add_test(tc_core, test_guid_add_token_second_uid);
  suite_add_tcase(s, tc_core);

  return s;
}

