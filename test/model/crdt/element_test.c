#include <check.h>
#include <stdlib.h>
#include <model/crdt/element.h>

START_TEST(test_key_from_tokens) {
  int result = key_from_tokens(3, 0, 2, 7);
  ck_assert_uint_eq(result, 60);
} END_TEST

START_TEST(test_uids_from_tokens) {
  int result;
  result = uids_from_tokens(3, 1, 1, 2);
  ck_assert_uint_eq(result, 8257);
  result = uids_from_tokens(4, 1, 1, 1, 1);
  ck_assert_uint_eq(result, 266305);
} END_TEST

START_TEST(test_add_token) {
  element l = {
    .depth = 3,
    .key = key_from_tokens(3, 0, 0, 0),
    .uids = uids_from_tokens(3, 1, 1, 1),
  };
  token t = { .key = 1, .uid = 1 };
  add_token(&l, t);

  element expected = {
    .depth = 4,
    .key = key_from_tokens(4, 0, 0, 0, 1),
    .uids = uids_from_tokens(4, 1, 1, 1, 1),
  };
  ck_assert_int_eq(l.depth, expected.depth);
  ck_assert_int_eq(l.key, expected.key);
  ck_assert_int_eq(l.uids, expected.uids);
} END_TEST

START_TEST(test_key_compare_siblings) {
  element l = {
    .depth = 3,
    .key = key_from_tokens(3, 0, 0, 0),
    .uids = uids_from_tokens(3, 1, 1, 1),
  };
  element r = {
    .depth = 3,
    .key = key_from_tokens(3, 0, 0, 2),
    .uids = uids_from_tokens(3, 1, 1, 1),
  };
  int result = key_compare(&l, &r);
  ck_assert_int_lt(result, 0);
  result = key_compare(&r, &l);
  ck_assert_int_gt(result, 0);
} END_TEST

START_TEST(test_key_compare_parent_child) {
  element l = {
    .depth = 3,
    .key = key_from_tokens(3, 0, 1, 3),
    .uids = uids_from_tokens(3, 1, 1, 1),
  };
  element r = {
    .depth = 4,
    .key = key_from_tokens(4, 0, 1, 3, 0),
    .uids = uids_from_tokens(4, 1, 1, 1, 1),
  };
  int result = key_compare(&l, &r);
  ck_assert_int_lt(result, 0);
  result = key_compare(&r, &l);
  ck_assert_int_gt(result, 0);
} END_TEST

START_TEST(test_key_compare_parent_second_child) {
  element l = {
    .depth = 3,
    .key = key_from_tokens(3, 0, 1, 3),
    .uids = uids_from_tokens(3, 1, 1, 1),
  };
  element r = {
    .depth = 4,
    .key = key_from_tokens(4, 0, 1, 3, 1),
    .uids = uids_from_tokens(4, 1, 1, 1, 1),
  };
  int result = key_compare(&l, &r);
  ck_assert_int_lt(result, 0);
  result = key_compare(&r, &l);
  ck_assert_int_gt(result, 0);
} END_TEST

START_TEST(test_key_compare_parent_grandchild) {
  element l = {
    .depth = 2,
    .key = key_from_tokens(2, 0, 1),
    .uids = uids_from_tokens(2, 1, 1),
  };
  element r = {
    .depth = 4,
    .key = key_from_tokens(4, 0, 1, 3, 1),
    .uids = uids_from_tokens(4, 1, 1, 1, 1),
  };
  int result = key_compare(&l, &r);
  ck_assert_int_lt(result, 0);
  result = key_compare(&r, &l);
  ck_assert_int_gt(result, 0);
} END_TEST

START_TEST(test_key_compare_uncle_nephew) {
  element l = {
    .depth = 2,
    .key = key_from_tokens(2, 0, 1),
    .uids = uids_from_tokens(2, 1, 1),
  };
  element r = {
    .depth = 3,
    .key = key_from_tokens(3, 0, 1, 4),
    .uids = uids_from_tokens(3, 1, 1, 1),
  };
  int result = key_compare(&l, &r);
  ck_assert_int_lt(result, 0);
  result = key_compare(&r, &l);
  ck_assert_int_gt(result, 0);
} END_TEST

START_TEST(test_key_compare_nephew_uncle) {
  element l = {
    .depth = 3,
    .key = key_from_tokens(3, 0, 1, 2),
    .uids = uids_from_tokens(2, 1, 1, 1),
  };
  element r = {
    .depth = 2,
    .key = key_from_tokens(2, 0, 3),
    .uids = uids_from_tokens(2, 1, 1),
  };
  int result = key_compare(&l, &r);
  ck_assert_int_lt(result, 0);
  result = key_compare(&r, &l);
  ck_assert_int_gt(result, 0);
} END_TEST

START_TEST(test_key_compare_different_user_parent) {
  element l = {
    .depth = 3,
    .key = key_from_tokens(3, 0, 1, 2),
    .uids = uids_from_tokens(2, 1, 1, 1),
  };
  element r = {
    .depth = 3,
    .key = key_from_tokens(3, 0, 1, 2),
    .uids = uids_from_tokens(2, 1, 2, 1),
  };
  int result = key_compare(&l, &r);
  ck_assert_int_lt(result, 0);
  result = key_compare(&r, &l);
  ck_assert_int_gt(result, 0);
} END_TEST

START_TEST(test_key_compare_different_user_sibling) {
  element l = {
    .depth = 3,
    .key = key_from_tokens(3, 0, 1, 2),
    .uids = uids_from_tokens(3, 1, 1, 1),
  };
  element r = {
    .depth = 3,
    .key = key_from_tokens(3, 0, 1, 2),
    .uids = uids_from_tokens(3, 1, 1, 2),
  };
  int result = key_compare(&l, &r);
  ck_assert_int_lt(result, 0);
  result = key_compare(&r, &l);
  ck_assert_int_gt(result, 0);
} END_TEST

START_TEST(test_key_compare_equal) {
  element l = {
    .depth = 2,
    .key = key_from_tokens(2, 0, 1),
    .uids = uids_from_tokens(2, 1, 1),
  };
  element r = {
    .depth = 2,
    .key = key_from_tokens(2, 0, 1),
    .uids = uids_from_tokens(2, 1, 1),
  };
  int result = key_compare(&l, &r);
  ck_assert_int_eq(result, 0);
  result = key_compare(&r, &l);
  ck_assert_int_eq(result, 0);
} END_TEST

START_TEST(test_key_equal) {
  element l = {
    .depth = 2,
    .key = key_from_tokens(2, 0, 1),
    .uids = uids_from_tokens(2, 1, 1),
  };
  element r = {
    .depth = 2,
    .key = key_from_tokens(2, 0, 1),
    .uids = uids_from_tokens(2, 1, 1),
  };
  int result = key_equal(&l, &r);
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
  tcase_add_test(tc_creating, test_key_from_tokens);
  tcase_add_test(tc_creating, test_uids_from_tokens);
  tcase_add_test(tc_creating, test_add_token);
  suite_add_tcase(s, tc_creating);

  // Comparing keys test case
  tcase_add_test(tc_comparing, test_key_compare_siblings);
  tcase_add_test(tc_comparing, test_key_compare_parent_child);
  tcase_add_test(tc_comparing, test_key_compare_parent_second_child);
  tcase_add_test(tc_comparing, test_key_compare_parent_grandchild);
  tcase_add_test(tc_comparing, test_key_compare_uncle_nephew);
  tcase_add_test(tc_comparing, test_key_compare_nephew_uncle);
  tcase_add_test(tc_comparing, test_key_compare_different_user_parent);
  tcase_add_test(tc_comparing, test_key_compare_different_user_sibling);
  tcase_add_test(tc_comparing, test_key_compare_equal);
  tcase_add_test(tc_comparing, test_key_equal);
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

