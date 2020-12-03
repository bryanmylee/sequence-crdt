#include <check.h>
#include <stdlib.h>

#include <element.h>

START_TEST(test_element_new) {
  Element *e = element_new();
  ck_assert_uint_eq(0, e->version);
  ck_assert_ptr_null(e->data.ptr);
  element_free(&e);
} END_TEST

START_TEST(test_element_free) {
  Element *e = element_new();
  int *x = malloc(sizeof(int));
  *x = 5;
  element_set_ptr(e, x, sizeof(int));
  element_free(&e);
  ck_assert_ptr_null(e);
} END_TEST

START_TEST(test_element_set_ptr) {
  Element *e = element_new();
  int *x = malloc(sizeof(int));
  *x = 5;

  element_set_ptr(e, x, sizeof(int));

  ck_assert_ptr_eq(x, e->data.ptr);
  ck_assert_int_eq(5, *(int *) e->data.ptr);
  ck_assert_int_eq(EREF, e->type);

  element_free(&e);
} END_TEST

START_TEST(test_element_get_ptr) {
  Element *e = element_new();
  int *x = malloc(sizeof(int));
  *x = 5;
  element_set_ptr(e, x, sizeof(int));

  ck_assert_int_eq(5, *(int *) element_get_ptr(e));

  element_free(&e);
} END_TEST

START_TEST(test_element_set_value) {
  Element *e = element_new();

  element_set_value(e, 5);

  ck_assert_int_eq(5, e->data.value);
  ck_assert_int_eq(EVALUE, e->type);

  element_free(&e);
} END_TEST

START_TEST(test_element_get_value) {
  Element *e = element_new();
  element_set_value(e, 5);

  ck_assert_int_eq(5, element_get_value(e));

  element_free(&e);
} END_TEST

Suite *element_suite(void) {
  Suite *s;
  TCase *tc_core;

  s = suite_create("check_element");
  tc_core = tcase_create("core");

  tcase_add_test(tc_core, test_element_new);
  tcase_add_test(tc_core, test_element_free);
  tcase_add_test(tc_core, test_element_get_ptr);
  tcase_add_test(tc_core, test_element_set_ptr);
  tcase_add_test(tc_core, test_element_get_value);
  tcase_add_test(tc_core, test_element_set_value);
  suite_add_tcase(s, tc_core);

  return s;
}

