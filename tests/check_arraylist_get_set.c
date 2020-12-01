#include <check.h>
#include <stdlib.h>

#include <arraylist.h>

START_TEST(test_al_get) {
  ArrayList* al = al_new(sizeof(int));
  int elements[10] = { 4, 5, 12, 512, 23, 50, 123, 77, 23, 10812 };
  for (int i = 0; i < 10; i++) {
    al_add(al, &elements[i]);
  }
  for (int i = 0; i < 10; i++) {
    ck_assert_int_eq(elements[i], *(int*) al_get(al, i));
  }
} END_TEST

Suite* arraylist_get_set_suite(void) {
  Suite *s;
  TCase *tc_get;
  TCase *tc_set;
  TCase *tc_boundary;

  s = suite_create("check_arraylist_insert_delete");
  tc_get = tcase_create("add");
  tc_set = tcase_create("remove");
  tc_boundary = tcase_create("boundary");

  tcase_add_test(tc_get, test_al_get);
  suite_add_tcase(s, tc_get);

  suite_add_tcase(s, tc_set);

  suite_add_tcase(s, tc_boundary);

  return s;
}

