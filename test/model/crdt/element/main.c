#include <check.h>
#include "./key_token_conv_test.c"

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

