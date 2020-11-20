#include <check.h>
#include <stdlib.h>
#include <util/bit.h>

START_TEST(test_print_bits) {
  int x = 51;
  char str[sizeof(x) * 8 + 1];
  sprint_bits(str, sizeof(x), &x);
  ck_assert_str_eq("00000000000000000000000000110011", str);
} END_TEST

START_TEST(test_bit_n_ones_c) {
  char result = bit_n_ones_c(5);
  char expected = 0b11111;
  ck_assert_int_eq(result, expected);
} END_TEST

START_TEST(test_bit_n_ones_i) {
  int result = bit_n_ones_i(18);
  int expected = 0b111111111111111111;
  ck_assert_int_eq(result, expected);
} END_TEST

Suite* bit_suite(void) {
  Suite *s;
  TCase *tc_core;

  s = suite_create("bit_suite");
  tc_core = tcase_create("core");

  tcase_add_test(tc_core, test_print_bits);
  tcase_add_test(tc_core, test_bit_n_ones_c);
  tcase_add_test(tc_core, test_bit_n_ones_i);
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

