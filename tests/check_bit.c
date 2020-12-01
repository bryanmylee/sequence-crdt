#include <check.h>
#include <stdlib.h>

#include <bit.h>

START_TEST(test_sprint_bits) {
  char *str = "abc";
  char result[100];
  print_bits(sizeof(char) * 4, str);
  sprint_bits(result, sizeof(char) * 4, str);
  ck_assert_str_eq("00000000011000110110001001100001", result);
} END_TEST

START_TEST(test_bit_n_ones_c) {
  ck_assert_int_eq(0b1, bit_n_ones_c(1));
  ck_assert_int_eq(0b1111, bit_n_ones_c(4));
  ck_assert_int_eq(0b11111111, bit_n_ones_c(8));
} END_TEST

START_TEST(test_bit_n_ones_i) {
  ck_assert_int_eq(0b1, bit_n_ones_i(1));
  ck_assert_int_eq(0b1111, bit_n_ones_i(4));
  ck_assert_int_eq(0b11111111, bit_n_ones_i(8));
  ck_assert_int_eq(0x7fff, bit_n_ones_i(15));
  ck_assert_uint_eq(0xffffffffUL, bit_n_ones_i(32));
} END_TEST

START_TEST(test_bit_n_ones_l) {
  ck_assert_int_eq(0b1, bit_n_ones_l(1));
  ck_assert_int_eq(0b1111, bit_n_ones_l(4));
  ck_assert_int_eq(0b11111111, bit_n_ones_l(8));
  ck_assert_int_eq(0x7fff, bit_n_ones_l(15));
  ck_assert_uint_eq(0xffffffffUL, bit_n_ones_l(32));
  ck_assert_uint_eq(0xffffffffffffUL, bit_n_ones_l(48));
  ck_assert_uint_eq(0xffffffffffffffffUL, bit_n_ones_l(64));
} END_TEST


Suite *bit_suite(void) {
  Suite *s;
  TCase *tc_core;

  s = suite_create("check_bit");
  tc_core = tcase_create("core");

  tcase_add_test(tc_core, test_sprint_bits);
  tcase_add_test(tc_core, test_bit_n_ones_c);
  tcase_add_test(tc_core, test_bit_n_ones_i);
  tcase_add_test(tc_core, test_bit_n_ones_l);
  suite_add_tcase(s, tc_core);

  return s;
}

