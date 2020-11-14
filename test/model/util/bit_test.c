#include <check.h>
#include <stdlib.h>
#include <util/bit.h>

START_TEST(test_print_bits) {
  int x = 51;
  print_bits(sizeof(x), &x);
  ck_assert(1);
} END_TEST

Suite* element_suite(void) {
  Suite *s;
  TCase *tc_core;

  s = suite_create("bit_suite");
  tc_core = tcase_create("core");

  tcase_add_test(tc_core, test_print_bits);
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

