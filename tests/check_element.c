#include <check.h>
#include <stdlib.h>

#include <element.h>

START_TEST(test_element_new) {
  Element *e = element_new();
  ck_assert_uint_eq(0, e->version);
  ck_assert_ptr_null(e->data.ptr);
  element_free(&e);
} END_TEST

START_TEST(test_element_free_ptr) {
  Element *e = element_new();
  e->data.ptr = malloc(sizeof(int));
  *(int *) e->data.ptr = 50;
  ck_assert_int_eq(50, *(int *) e->data.ptr);
  element_free_ptr(&e);
  ck_assert_ptr_null(e);
} END_TEST

Suite *element_suite(void) {
  Suite *s;
  TCase *tc_core;

  s = suite_create("check_element");
  tc_core = tcase_create("core");

  tcase_add_test(tc_core, test_element_new);
  tcase_add_test(tc_core, test_element_free_ptr);
  suite_add_tcase(s, tc_core);

  return s;
}

