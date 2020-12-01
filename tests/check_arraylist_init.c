#include <check.h>
#include <stdlib.h>

#include <arraylist.h>

START_TEST(test_al_init) {
  ArrayList al;
  al_init(&al, sizeof(int));
  ck_assert_int_eq(al.cap, 16);
  ck_assert_int_eq(al.size, 0);
  al_free_internal(&al);
} END_TEST

START_TEST(test_al_new) {
  ArrayList *al = al_new(sizeof(int));
  ck_assert_int_eq(al->cap, 16);
  ck_assert_int_eq(al->size, 0);
  free(al);
} END_TEST

START_TEST(test_al_free) {
  ArrayList *al = al_new(sizeof(int));
  al_free(&al);
  ck_assert_ptr_null(al);
} END_TEST

Suite *arraylist_init_suite(void) {
  Suite *s;
  TCase *tc_init;

  s = suite_create("check_arraylist_init");
  tc_init = tcase_create("init");
  tcase_add_test(tc_init, test_al_init);
  tcase_add_test(tc_init, test_al_new);
  tcase_add_test(tc_init, test_al_free);
  suite_add_tcase(s, tc_init);

  return s;
}
