#include <check.h>
#include <stdlib.h>
#include <model/crdt/sequence.h>

START_TEST(test_get_token_between) {
  token l = { .key = 4, .uid = 1 };
  token r = { .key = 6, .uid = 1 };
  token expected = { .key = 5, .uid = 1 };
  token result = get_token_between(&l, &r, 3, 1);
  ck_assert_uint_eq(result.key, expected.key);
  ck_assert_int_eq(result.uid, expected.uid);
} END_TEST

START_TEST(test_get_token_between_no_space) {
  token l = { .key = 4, .uid = 1 };
  token r = { .key = 5, .uid = 1 };
  token expected = { .key = -1, .uid = -1 };
  token result = get_token_between(&l, &r, 3, 1);
  ck_assert_uint_eq(result.key, expected.key);
  ck_assert_int_eq(result.uid, expected.uid);
} END_TEST

START_TEST(test_get_guid_between_siblings) {
  guid l = {
    .depth = 3,
    .keys = keys_from_tokens(3, 1, 2, 1),
    .uids = uids_from_tokens(3, 1, 1, 1),
  };
  guid r = {
    .depth = 3,
    .keys = keys_from_tokens(3, 1, 2, 3),
    .uids = uids_from_tokens(3, 1, 1, 1),
  };
  guid expected = {
    .depth = 3,
    .keys = keys_from_tokens(3, 1, 2, 2),
    .uids = uids_from_tokens(3, 1, 1, 2),
  };
  guid result = get_guid_between(&l, &r, 2);
  ck_assert_int_eq(result.depth, expected.depth);
  ck_assert_uint_eq(result.keys, expected.keys);
  ck_assert_int_eq(result.uids, expected.uids);
} END_TEST

Suite* element_suite(void) {
  Suite *s;
  TCase *tc_guid;

  s = suite_create("element_suite");
  tc_guid = tcase_create("guid");

  tcase_add_test(tc_guid, test_get_token_between);
  tcase_add_test(tc_guid, test_get_token_between_no_space);
  tcase_add_test(tc_guid, test_get_guid_between_siblings);
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

