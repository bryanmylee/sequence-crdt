#include <check.h>
#include <stdlib.h>
#include <model/crdt/element.h>

START_TEST(test_key_from_tokens) {
  int result = key_from_tokens(3, 0, 2, 7);
  ck_assert_uint_eq(60, result);
} END_TEST

Suite* element_suite(void) {
  Suite *s;
  TCase *tc_core;

  s = suite_create("element_suite");
  tc_core = tcase_create("core");

  tcase_add_test(tc_core, test_key_from_tokens);
  suite_add_tcase(s, tc_core);

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

