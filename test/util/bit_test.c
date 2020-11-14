#include <check.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/bit.h>

START_TEST(test_print_bits) {
  int x = 51;
  char str[sizeof(x) * 8 + 1];
  sprint_bits(str, sizeof(x), &x);
  ck_assert_str_eq("00000000000000000000000000110011", str);
} END_TEST

Suite* bit_suite(void) {
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

  s = bit_suite();
  runner = srunner_create(s);

  srunner_run_all(runner, CK_NORMAL);
  no_failed = srunner_ntests_failed(runner);
  srunner_free(runner);
  return (no_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

