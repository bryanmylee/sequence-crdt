#include <check.h>
#include <stdlib.h>
#include <model/crdt/element.h>

START_TEST(test_key_from_tokens) {
  int result = key_from_tokens(3, 0, 2, 7);
  ck_assert_uint_eq(result, 60);
} END_TEST

START_TEST(test_uids_from_tokens) {
  int result = uids_from_tokens(3, 1, 1, 2);
  ck_assert_uint_eq(result, 8257);
} END_TEST

START_TEST(test_key_compare_siblings) {
  element l = { .key = 0, .depth = 3 };
  element r = { .key = 16, .depth = 3 };
  int result = key_compare(&l, &r);
  ck_assert_int_gt(result, 0);
  result = key_compare(&r, &l);
  ck_assert_int_lt(result, 0);
} END_TEST

START_TEST(test_key_compare_parent_child) {
  element l = { .key = 0, .depth = 3 };
  element r = { .key = 0, .depth = 4 };
  int result = key_compare(&l, &r);
  ck_assert_int_gt(result, 0);
  result = key_compare(&r, &l);
  ck_assert_int_lt(result, 0);
} END_TEST

START_TEST(test_key_compare_parent_second_child) {
  element l = { .key = 0, .depth = 2 };
  element r = { .key = 8, .depth = 3 };
  int result = key_compare(&l, &r);
  ck_assert_int_gt(result, 0);
  result = key_compare(&r, &l);
  ck_assert_int_lt(result, 0);
} END_TEST

START_TEST(test_key_compare_parent_grandchild) {
  element l = { .key = 0, .depth = 1 };
  element r = { .key = 8, .depth = 3 };
  int result = key_compare(&l, &r);
  ck_assert_int_gt(result, 0);
  result = key_compare(&r, &l);
  ck_assert_int_lt(result, 0);
} END_TEST

START_TEST(test_key_compare_uncle_nephew) {
  element l = { .key = 4, .depth = 3 };
  element r = { .key = 5, .depth = 2 };
  int result = key_compare(&l, &r);
  ck_assert_int_gt(result, 0);
  result = key_compare(&r, &l);
  ck_assert_int_lt(result, 0);
} END_TEST

START_TEST(test_key_compare_uncle_nephew_opposite) {
  element l = { .key = 4, .depth = 2 };
  element r = { .key = 5, .depth = 3 };
  int result = key_compare(&l, &r);
  ck_assert_int_gt(result, 0);
  result = key_compare(&r, &l);
  ck_assert_int_lt(result, 0);
} END_TEST

START_TEST(test_key_compare_equal) {
  element l = { .key = 4, .depth = 2 };
  element r = { .key = 4, .depth = 2 };
  int result = key_compare(&l, &r);
  ck_assert_int_eq(result, 0);
  result = key_compare(&r, &l);
  ck_assert_int_eq(result, 0);
} END_TEST

START_TEST(test_key_equal) {
  element l = { .key = 4, .depth = 2 };
  element r = { .key = 4, .depth = 2 };
  int result = key_equal(&l, &r);
  ck_assert_int_eq(result, 1);
} END_TEST

Suite* element_suite(void) {
  Suite *s;
  TCase *tc_converting;
  TCase *tc_comparing;

  s = suite_create("element_suite");
  tc_converting = tcase_create("converting");
  tc_comparing = tcase_create("comparing");

  // Conversion of key test case
  tcase_add_test(tc_converting, test_key_from_tokens);
  tcase_add_test(tc_converting, test_uids_from_tokens);
  suite_add_tcase(s, tc_converting);

  // Comparing keys test case
  tcase_add_test(tc_comparing, test_key_compare_siblings);
  tcase_add_test(tc_comparing, test_key_compare_parent_child);
  tcase_add_test(tc_comparing, test_key_compare_parent_second_child);
  tcase_add_test(tc_comparing, test_key_compare_parent_grandchild);
  tcase_add_test(tc_comparing, test_key_compare_uncle_nephew);
  tcase_add_test(tc_comparing, test_key_compare_uncle_nephew_opposite);
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

