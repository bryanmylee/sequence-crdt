#include <check.h>
#include <stdlib.h>
#include <model/crdt/guid.h>

START_TEST(test_keys_from_tokens) {
  int result;
  result = keys_from_tokens(3, 0, 2, 7);
  ck_assert_uint_eq(result, 60);
  result = keys_from_tokens(3, 0, 1, 2);
  ck_assert_uint_eq(result, 18);
} END_TEST

START_TEST(test_uids_from_tokens) {
  int result;
  result = uids_from_tokens(3, 1, 1, 2);
  ck_assert_uint_eq(result, 8257);
  result = uids_from_tokens(4, 1, 1, 1, 1);
  ck_assert_uint_eq(result, 266305);
} END_TEST

START_TEST(test_guid_add_token) {
  guid g = {
    .depth = 3,
    .keys = keys_from_tokens(3, 0, 0, 0),
    .uids = uids_from_tokens(3, 1, 1, 1),
  };
  token t = { .key = 1, .uid = 1 };
  guid_add_token(&g, t);

  guid expected = {
    .depth = 4,
    .keys = keys_from_tokens(4, 0, 0, 0, 1),
    .uids = uids_from_tokens(4, 1, 1, 1, 1),
  };
  ck_assert_int_eq(g.depth, expected.depth);
  ck_assert_int_eq(g.keys, expected.keys);
  ck_assert_int_eq(g.uids, expected.uids);
} END_TEST

START_TEST(test_guid_add_token_second_uid) {
  guid g = {
    .depth = 3,
    .keys = keys_from_tokens(3, 0, 0, 0),
    .uids = uids_from_tokens(3, 1, 1, 1),
  };
  token t = { .key = 1, .uid = 2 };
  guid_add_token(&g, t);

  guid expected = {
    .depth = 4,
    .keys = keys_from_tokens(4, 0, 0, 0, 1),
    .uids = uids_from_tokens(4, 1, 1, 1, 2),
  };
  ck_assert_int_eq(g.depth, expected.depth);
  ck_assert_int_eq(g.keys, expected.keys);
  ck_assert_int_eq(g.uids, expected.uids);
} END_TEST

START_TEST(test_guid_copy) {
  unsigned long orig_keys = keys_from_tokens(3, 0, 1, 3);
  guid orig = {
    .depth = 3,
    .keys = orig_keys,
    .uids = uids_from_tokens(3, 1, 2, 1),
  };
  guid copy = guid_copy(&orig);
  orig.keys = keys_from_tokens(3, 0, 1, 4);

  ck_assert_int_eq(copy.keys, orig_keys);
} END_TEST

START_TEST(test_guid_compare_siblings) {
  guid l = {
    .depth = 3,
    .keys = keys_from_tokens(3, 0, 0, 0),
    .uids = uids_from_tokens(3, 1, 1, 1),
  };
  guid r = {
    .depth = 3,
    .keys = keys_from_tokens(3, 0, 0, 2),
    .uids = uids_from_tokens(3, 1, 1, 1),
  };
  int result = guid_compare(&l, &r);
  ck_assert_int_lt(result, 0);
  result = guid_compare(&r, &l);
  ck_assert_int_gt(result, 0);
} END_TEST

START_TEST(test_guid_compare_parent_child) {
  guid l = {
    .depth = 3,
    .keys = keys_from_tokens(3, 0, 1, 3),
    .uids = uids_from_tokens(3, 1, 1, 1),
  };
  guid r = {
    .depth = 4,
    .keys = keys_from_tokens(4, 0, 1, 3, 0),
    .uids = uids_from_tokens(4, 1, 1, 1, 1),
  };
  int result = guid_compare(&l, &r);
  ck_assert_int_lt(result, 0);
  result = guid_compare(&r, &l);
  ck_assert_int_gt(result, 0);
} END_TEST

START_TEST(test_guid_compare_parent_second_child) {
  guid l = {
    .depth = 3,
    .keys = keys_from_tokens(3, 0, 1, 3),
    .uids = uids_from_tokens(3, 1, 1, 1),
  };
  guid r = {
    .depth = 4,
    .keys = keys_from_tokens(4, 0, 1, 3, 1),
    .uids = uids_from_tokens(4, 1, 1, 1, 1),
  };
  int result = guid_compare(&l, &r);
  ck_assert_int_lt(result, 0);
  result = guid_compare(&r, &l);
  ck_assert_int_gt(result, 0);
} END_TEST

START_TEST(test_guid_compare_parent_grandchild) {
  guid l = {
    .depth = 2,
    .keys = keys_from_tokens(2, 0, 1),
    .uids = uids_from_tokens(2, 1, 1),
  };
  guid r = {
    .depth = 4,
    .keys = keys_from_tokens(4, 0, 1, 3, 1),
    .uids = uids_from_tokens(4, 1, 1, 1, 1),
  };
  int result = guid_compare(&l, &r);
  ck_assert_int_lt(result, 0);
  result = guid_compare(&r, &l);
  ck_assert_int_gt(result, 0);
} END_TEST

START_TEST(test_guid_compare_uncle_nephew) {
  guid l = {
    .depth = 2,
    .keys = keys_from_tokens(2, 0, 1),
    .uids = uids_from_tokens(2, 1, 1),
  };
  guid r = {
    .depth = 3,
    .keys = keys_from_tokens(3, 0, 1, 4),
    .uids = uids_from_tokens(3, 1, 1, 1),
  };
  int result = guid_compare(&l, &r);
  ck_assert_int_lt(result, 0);
  result = guid_compare(&r, &l);
  ck_assert_int_gt(result, 0);
} END_TEST

START_TEST(test_guid_compare_nephew_uncle) {
  guid l = {
    .depth = 3,
    .keys = keys_from_tokens(3, 0, 1, 2),
    .uids = uids_from_tokens(2, 1, 1, 1),
  };
  guid r = {
    .depth = 2,
    .keys = keys_from_tokens(2, 0, 3),
    .uids = uids_from_tokens(2, 1, 1),
  };
  int result = guid_compare(&l, &r);
  ck_assert_int_lt(result, 0);
  result = guid_compare(&r, &l);
  ck_assert_int_gt(result, 0);
} END_TEST

START_TEST(test_guid_compare_different_user_parent) {
  guid l = {
    .depth = 3,
    .keys = keys_from_tokens(3, 0, 1, 2),
    .uids = uids_from_tokens(2, 1, 1, 1),
  };
  guid r = {
    .depth = 3,
    .keys = keys_from_tokens(3, 0, 1, 2),
    .uids = uids_from_tokens(2, 1, 2, 1),
  };
  int result = guid_compare(&l, &r);
  ck_assert_int_lt(result, 0);
  result = guid_compare(&r, &l);
  ck_assert_int_gt(result, 0);
} END_TEST

START_TEST(test_guid_compare_different_user_sibling) {
  guid l = {
    .depth = 3,
    .keys = keys_from_tokens(3, 0, 1, 2),
    .uids = uids_from_tokens(3, 1, 1, 1),
  };
  guid r = {
    .depth = 3,
    .keys = keys_from_tokens(3, 0, 1, 2),
    .uids = uids_from_tokens(3, 1, 1, 2),
  };
  int result = guid_compare(&l, &r);
  ck_assert_int_lt(result, 0);
  result = guid_compare(&r, &l);
  ck_assert_int_gt(result, 0);
} END_TEST

START_TEST(test_guid_compare_equal) {
  guid l = {
    .depth = 2,
    .keys = keys_from_tokens(2, 0, 1),
    .uids = uids_from_tokens(2, 1, 1),
  };
  guid r = {
    .depth = 2,
    .keys = keys_from_tokens(2, 0, 1),
    .uids = uids_from_tokens(2, 1, 1),
  };
  int result = guid_compare(&l, &r);
  ck_assert_int_eq(result, 0);
  result = guid_compare(&r, &l);
  ck_assert_int_eq(result, 0);
} END_TEST

START_TEST(test_guid_equal) {
  guid l = {
    .depth = 2,
    .keys = keys_from_tokens(2, 0, 1),
    .uids = uids_from_tokens(2, 1, 1),
  };
  guid r = {
    .depth = 2,
    .keys = keys_from_tokens(2, 0, 1),
    .uids = uids_from_tokens(2, 1, 1),
  };
  int result = guid_equal(&l, &r);
  ck_assert_int_eq(result, true);
} END_TEST

Suite* element_suite(void) {
  Suite *s;
  TCase *tc_creating;
  TCase *tc_comparing;

  s = suite_create("element_suite");
  tc_creating = tcase_create("creating");
  tc_comparing = tcase_create("comparing");

  // Creation of key test case
  tcase_add_test(tc_creating, test_keys_from_tokens);
  tcase_add_test(tc_creating, test_uids_from_tokens);
  tcase_add_test(tc_creating, test_guid_add_token);
  tcase_add_test(tc_creating, test_guid_add_token_second_uid);
  tcase_add_test(tc_creating, test_guid_copy);
  suite_add_tcase(s, tc_creating);

  // Comparing keys test case
  tcase_add_test(tc_comparing, test_guid_compare_siblings);
  tcase_add_test(tc_comparing, test_guid_compare_parent_child);
  tcase_add_test(tc_comparing, test_guid_compare_parent_second_child);
  tcase_add_test(tc_comparing, test_guid_compare_parent_grandchild);
  tcase_add_test(tc_comparing, test_guid_compare_uncle_nephew);
  tcase_add_test(tc_comparing, test_guid_compare_nephew_uncle);
  tcase_add_test(tc_comparing, test_guid_compare_different_user_parent);
  tcase_add_test(tc_comparing, test_guid_compare_different_user_sibling);
  tcase_add_test(tc_comparing, test_guid_compare_equal);
  tcase_add_test(tc_comparing, test_guid_equal);
  suite_add_tcase(s, tc_comparing);

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

