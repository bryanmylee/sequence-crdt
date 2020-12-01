#include <check.h>
#include <stdlib.h>

#include <guid.h>

START_TEST(test_guid_compare_siblings) {
  Guid l = {
    .depth = 3,
    .keys = keys_from_tokens(3, 0, 0, 0),
    .uids = uids_from_tokens(3, 1, 1, 1),
  };
  Guid r = {
    .depth = 3,
    .keys = keys_from_tokens(3, 0, 0, 2),
    .uids = uids_from_tokens(3, 1, 1, 1),
  };
  int result = guid_compare(&l, &r);
  ck_assert_int_lt(result, 0);
  result = guid_compare(&r, &l);
  ck_assert_int_gt(result, 0);
} END_TEST

START_TEST(test_guid_compare_siblings_nested) {
  Guid l = {
    .depth = 5,
    .keys = keys_from_tokens(5, 0, 1, 2, 4, 6),
    .uids = uids_from_tokens(5, 1, 1, 1, 1, 1),
  };
  Guid r = {
    .depth = 5,
    .keys = keys_from_tokens(5, 0, 1, 2, 4, 10),
    .uids = uids_from_tokens(5, 1, 1, 1, 1, 1),
  };
  int result = guid_compare(&l, &r);
  ck_assert_int_lt(result, 0);
  result = guid_compare(&r, &l);
  ck_assert_int_gt(result, 0);
} END_TEST

START_TEST(test_guid_compare_parent_child) {
  Guid l = {
    .depth = 3,
    .keys = keys_from_tokens(3, 0, 1, 3),
    .uids = uids_from_tokens(3, 1, 1, 1),
  };
  Guid r = {
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
  Guid l = {
    .depth = 3,
    .keys = keys_from_tokens(3, 0, 1, 3),
    .uids = uids_from_tokens(3, 1, 1, 1),
  };
  Guid r = {
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
  Guid l = {
    .depth = 2,
    .keys = keys_from_tokens(2, 0, 1),
    .uids = uids_from_tokens(2, 1, 1),
  };
  Guid r = {
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
  Guid l = {
    .depth = 2,
    .keys = keys_from_tokens(2, 0, 1),
    .uids = uids_from_tokens(2, 1, 1),
  };
  Guid r = {
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
  Guid l = {
    .depth = 3,
    .keys = keys_from_tokens(3, 0, 1, 2),
    .uids = uids_from_tokens(2, 1, 1, 1),
  };
  Guid r = {
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
  Guid l = {
    .depth = 3,
    .keys = keys_from_tokens(3, 0, 1, 2),
    .uids = uids_from_tokens(2, 1, 1, 1),
  };
  Guid r = {
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
  Guid l = {
    .depth = 3,
    .keys = keys_from_tokens(3, 0, 1, 2),
    .uids = uids_from_tokens(3, 1, 1, 1),
  };
  Guid r = {
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
  Guid l = {
    .depth = 2,
    .keys = keys_from_tokens(2, 0, 1),
    .uids = uids_from_tokens(2, 1, 1),
  };
  Guid r = {
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
  Guid l = {
    .depth = 2,
    .keys = keys_from_tokens(2, 0, 1),
    .uids = uids_from_tokens(2, 1, 1),
  };
  Guid r = {
    .depth = 2,
    .keys = keys_from_tokens(2, 0, 1),
    .uids = uids_from_tokens(2, 1, 1),
  };
  int result = guid_equal(&l, &r);
  ck_assert_int_eq(result, true);
} END_TEST

Suite *guid_compare_suite(void) {
  Suite *s;
  TCase *tc_core;

  s = suite_create("check_guid_compare");
  tc_core = tcase_create("core");

  // Comparing keys test case
  tcase_add_test(tc_core, test_guid_compare_siblings);
  tcase_add_test(tc_core, test_guid_compare_siblings_nested);
  tcase_add_test(tc_core, test_guid_compare_parent_child);
  tcase_add_test(tc_core, test_guid_compare_parent_second_child);
  tcase_add_test(tc_core, test_guid_compare_parent_grandchild);
  tcase_add_test(tc_core, test_guid_compare_uncle_nephew);
  tcase_add_test(tc_core, test_guid_compare_nephew_uncle);
  tcase_add_test(tc_core, test_guid_compare_different_user_parent);
  tcase_add_test(tc_core, test_guid_compare_different_user_sibling);
  tcase_add_test(tc_core, test_guid_compare_equal);
  tcase_add_test(tc_core, test_guid_equal);
  suite_add_tcase(s, tc_core);

  return s;
}

