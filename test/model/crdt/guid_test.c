#include <check.h>
#include <stdlib.h>
#include <model/crdt/guid.h>

START_TEST(test_guid_init) {
  Guid g;
  guid_init(&g);
  ck_assert_int_eq(g.keys, 0);
  ck_assert_int_eq(g.uids, 0);
  ck_assert_int_eq(g.depth, 0);
}

START_TEST(test_guid_new) {
  Guid* g = guid_new();
  ck_assert_int_eq(g->keys, 0);
  ck_assert_int_eq(g->uids, 0);
  ck_assert_int_eq(g->depth, 0);
  free(g);
}

START_TEST(test_guid_copy_into) {
  unsigned long orig_keys = keys_from_tokens(3, 0, 1, 3);
  Guid orig = {
    .depth = 3,
    .keys = orig_keys,
    .uids = uids_from_tokens(3, 1, 2, 1),
  };
  Guid copy;
  guid_copy_into(&copy, &orig);
  orig.keys = keys_from_tokens(3, 0, 1, 4);

  ck_assert_int_eq(copy.keys, orig_keys);
} END_TEST

START_TEST(test_guid_free) {
  Guid* g = guid_new();
  ck_assert_int_eq(g->depth, 0);
  ck_assert_uint_eq(g->keys, 0);
  ck_assert_uint_eq(g->uids, 0);
  guid_free(&g);
  ck_assert_ptr_null(g);
} END_TEST

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
  Guid g = {
    .depth = 3,
    .keys = keys_from_tokens(3, 0, 0, 0),
    .uids = uids_from_tokens(3, 1, 1, 1),
  };
  token t = { .key = 1, .uid = 1 };
  guid_add_token(&g, t);

  Guid expected = {
    .depth = 4,
    .keys = keys_from_tokens(4, 0, 0, 0, 1),
    .uids = uids_from_tokens(4, 1, 1, 1, 1),
  };
  ck_assert_int_eq(g.depth, expected.depth);
  ck_assert_int_eq(g.keys, expected.keys);
  ck_assert_int_eq(g.uids, expected.uids);
} END_TEST

START_TEST(test_guid_add_token_second_uid) {
  Guid g = {
    .depth = 3,
    .keys = keys_from_tokens(3, 0, 0, 0),
    .uids = uids_from_tokens(3, 1, 1, 1),
  };
  token t = { .key = 1, .uid = 2 };
  guid_add_token(&g, t);

  Guid expected = {
    .depth = 4,
    .keys = keys_from_tokens(4, 0, 0, 0, 1),
    .uids = uids_from_tokens(4, 1, 1, 1, 2),
  };
  ck_assert_int_eq(g.depth, expected.depth);
  ck_assert_int_eq(g.keys, expected.keys);
  ck_assert_int_eq(g.uids, expected.uids);
} END_TEST

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

Suite* guid_suite(void) {
  Suite *s;
  TCase *tc_memory;
  TCase *tc_comparing;

  s = suite_create("guid_suite");
  tc_memory = tcase_create("memory");
  tc_comparing = tcase_create("comparing");

  // Creation of key test case
  tcase_add_test(tc_memory, test_guid_init);
  tcase_add_test(tc_memory, test_guid_new);
  tcase_add_test(tc_memory, test_guid_copy_into);
  tcase_add_test(tc_memory, test_guid_free);
  tcase_add_test(tc_memory, test_keys_from_tokens);
  tcase_add_test(tc_memory, test_uids_from_tokens);
  tcase_add_test(tc_memory, test_guid_add_token);
  tcase_add_test(tc_memory, test_guid_add_token_second_uid);
  suite_add_tcase(s, tc_memory);

  // Comparing keys test case
  tcase_add_test(tc_comparing, test_guid_compare_siblings);
  tcase_add_test(tc_comparing, test_guid_compare_siblings_nested);
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

  s = guid_suite();
  runner = srunner_create(s);

  srunner_run_all(runner, CK_NORMAL);
  no_failed = srunner_ntests_failed(runner);
  srunner_free(runner);
  return (no_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

