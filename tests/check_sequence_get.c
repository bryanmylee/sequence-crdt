#include <check.h>
#include <stdlib.h>

#include <sequence.h>

START_TEST(test_seq_get) {
  Sequence *s = seq_new();

  char *data = "this is a string";
  int n = strlen(data);
  for (int i = 0; i < n; i++) {
    seq_insert_value(s, data[i], i);
  }

  Element *result = seq_get_element(s, 12);
  ck_assert_int_eq('r', (char) element_get_value(result));

  // last character
  result = seq_get_element(s, n - 1);
  ck_assert_int_eq('g', (char) element_get_value(result));

  seq_free(&s);
} END_TEST

START_TEST(test_seq_get_out_of_bounds_positive) {
  Sequence *s = seq_new();

  char *data = "this is a string";
  int n = strlen(data);
  for (int i = 0; i < n; i++) {
    seq_insert_value(s, data[i], i);
  }

  Element *result = seq_get_element(s, n);
  ck_assert_ptr_null(result);

  seq_free(&s);
} END_TEST

START_TEST(test_seq_get_out_of_bounds_negative) {
  Sequence *s = seq_new();

  char *data = "this is a string";
  int n = strlen(data);
  for (int i = 0; i < n; i++) {
    seq_insert_value(s, data[i], i);
  }

  Element *result = seq_get_element(s, -1);
  ck_assert_ptr_null(result);

  seq_free(&s);
} END_TEST


Suite *sequence_get_suite(void) {
  Suite *s;
  TCase *tc_core;
  TCase *tc_boundary;

  s = suite_create("check_sequence_get");
  tc_core = tcase_create("core");
  tc_boundary = tcase_create("boundary");

  tcase_add_test(tc_core, test_seq_get);
  suite_add_tcase(s, tc_core);

  tcase_add_test(tc_boundary, test_seq_get_out_of_bounds_positive);
  tcase_add_test(tc_boundary, test_seq_get_out_of_bounds_negative);
  suite_add_tcase(s, tc_boundary);

  return s;
}
