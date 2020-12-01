#include <check.h>
#include <stdlib.h>

#include <bit.h>

START_TEST(test_sprint_bits) {
  char *str = "abc";
  char result[100];
  sprint_bits(result, sizeof(char) * 4, str);
  ck_assert_str_eq("00000000011000110110001001100001", result);
} END_TEST

Suite *bit_suite(void) {
  Suite *s;
  TCase *tc_core;

  s = suite_create("check_bit");
  tc_core = tcase_create("core");

  tcase_add_test(tc_core, test_sprint_bits);
  suite_add_tcase(s, tc_core);

  return s;
}

