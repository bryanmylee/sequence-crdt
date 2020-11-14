#include <check.h>
#include <stdlib.h>
#include <model/arraylist.h>

START_TEST(test_al_init) {
  arraylist al;
  al_init(&al);
  ck_assert_int_eq(al.cap, 16);
  ck_assert_int_eq(al.size, 0);
} END_TEST

START_TEST(test_al_add_expand) {
  arraylist al;
  al_init(&al);

  for (int i = 0; i < 20; i++) {
    int* e = malloc(sizeof(int));
    *e = i;
    al_add(&al, e);
  }

  ck_assert_int_eq(al.cap, 32);
  ck_assert_int_eq(al.size, 20);
  for (int i = 0; i < al.size; i++) {
    ck_assert_int_eq(*((int*) al.data[i]), i);
  }
}

Suite* arraylist_suite(void) {
  Suite *s;
  TCase *tc_core;

  s = suite_create("arraylist_suite");
  tc_core = tcase_create("core");

  tcase_add_test(tc_core, test_al_init);
  tcase_add_test(tc_core, test_al_add_expand);
  suite_add_tcase(s, tc_core);

  return s;
}

int main(void) {
  int no_failed = 0;
  Suite *s;
  SRunner *runner;

  s = arraylist_suite();
  runner = srunner_create(s);

  srunner_run_all(runner, CK_NORMAL);
  no_failed = srunner_ntests_failed(runner);
  srunner_free(runner);
  return (no_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

