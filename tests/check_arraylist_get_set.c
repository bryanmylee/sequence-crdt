#include <check.h>
#include <stdlib.h>

#include <arraylist.h>

START_TEST(test_al_get) {
  ArrayList *al = al_new(sizeof(int));
  int elements[10] = { 4, 5, 12, 512, 23, 50, 123, 77, 23, 10812 };
  for (int i = 0; i < 10; i++) {
    al_add(al, &elements[i]);
  }
  for (int i = 0; i < 10; i++) {
    ck_assert_int_eq(elements[i], *(int*) al_get(al, i));
  }
  al_free(&al);
} END_TEST

START_TEST(test_al_set) {
  ArrayList *al = al_new(sizeof(int));
  int old_elements[10] = { 0 };
  for (int i = 0; i < 10; i++) {
    al_add(al, &old_elements[i]);
  }
  int elements[10] = { 4, 5, 12, 512, 23, 50, 123, 77, 23, 10812 };
  for (int i = 0; i < 10; i++) {
    al_set(al, i, &elements[i]);
  }
  for (int i = 0; i < 10; i++) {
    ck_assert_int_eq(elements[i], *(int*) al_get(al, i));
  }
  al_free(&al);
} END_TEST

START_TEST(test_al_get_out_of_bounds) {
  ArrayList *al = al_new(sizeof(int));
  int elements[10] = { 4, 5, 12, 512, 23, 50, 123, 77, 23, 10812 };
  for (int i = 0; i < 10; i++) {
    al_add(al, &elements[i]);
  }
  ck_assert_ptr_null(al_get(al, -1));
  ck_assert_ptr_null(al_get(al, 10));
  al_free(&al);
} END_TEST

START_TEST(test_al_set_out_of_bounds) {
  ArrayList *al = al_new(sizeof(int));
  int old_elements[10] = { 0 };
  for (int i = 0; i < 10; i++) {
    al_add(al, &old_elements[i]);
  }

  int x = 10;
  al_set(al, -1, &x);
  al_set(al, 10, &x);

  for (int i = 0; i < 10; i++) {
    ck_assert_int_eq(0, *(int*) al_get(al, i));
  }
  al_free(&al);
} END_TEST

Suite *arraylist_get_set_suite(void) {
  Suite *s;
  TCase *tc_core;
  TCase *tc_boundary;

  s = suite_create("check_arraylist_insert_delete");
  tc_core = tcase_create("core");
  tc_boundary = tcase_create("boundary");

  tcase_add_test(tc_core, test_al_get);
  tcase_add_test(tc_core, test_al_set);
  suite_add_tcase(s, tc_core);

  tcase_add_test(tc_boundary, test_al_get_out_of_bounds);
  tcase_add_test(tc_boundary, test_al_set_out_of_bounds);
  suite_add_tcase(s, tc_boundary);

  return s;
}

