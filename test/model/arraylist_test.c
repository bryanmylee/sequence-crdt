#include <check.h>
#include <stdio.h>
#include <stdlib.h>
#include <model/arraylist.h>

START_TEST(test_al_init) {
  arraylist al;
  al_init(&al);
  ck_assert_int_eq(al.cap, 16);
  ck_assert_int_eq(al.size, 0);
} END_TEST

START_TEST(test_al_add_at_expand) {
  arraylist al;
  al_init(&al);

  for (int i = 0; i < 20; i++) {
    int* e = malloc(sizeof(int));
    *e = i;
    al_add_at(&al, e, 0);
  }

  ck_assert_int_eq(al.cap, 32);
  ck_assert_int_eq(al.size, 20);
  for (int i = 0; i < al.size; i++) {
    ck_assert_int_eq(*((int*) al.data[al.size - 1 - i]), i);
  }
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
} END_TEST

START_TEST(test_al_add_all_at_expand) {
  arraylist al;
  al_init(&al);

  for (int i = 0; i < 20; i++) {
    int* e = malloc(sizeof(int));
    *e = i;
    al_add(&al, e);
  }
  // insert 20 zeros at index 10.
  int* es[20];
  for (int i = 0; i < 20; i++) {
    int* e = malloc(sizeof(int));
    *e = 0;
    es[i] = e;
  }
  al_add_all_at(&al, (void**) es, 20, 10);
  ck_assert_int_eq(al.cap, 64);
  ck_assert_int_eq(al.size, 40);
  int expected[] = {
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, \
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0, \
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0, \
    10, 11, 12, 13, 14, 15, 16, 17, 18, 19  \
  };
  for (int i = 0; i < al.size; i++) {
    int e = *((int*) al.data[i]);
    ck_assert_int_eq(e, expected[i]);
  }
} END_TEST

START_TEST(test_al_add_all_at_empty) {
  arraylist al;
  al_init(&al);

  // insert 20 zeros at index 0.
  int* es[20];
  for (int i = 0; i < 20; i++) {
    int* e = malloc(sizeof(int));
    *e = 0;
    es[i] = e;
  }
  al_add_all_at(&al, (void**) es, 20, 0);
  ck_assert_int_eq(al.cap, 32);
  ck_assert_int_eq(al.size, 20);
  int expected[] = {
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0, \
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0  \
  };
  for (int i = 0; i < al.size; i++) {
    int e = *((int*) al.data[i]);
    ck_assert_int_eq(e, expected[i]);
  }
} END_TEST

START_TEST(test_al_add_all_expand) {
  arraylist al;
  al_init(&al);

  for (int i = 0; i < 20; i++) {
    int* e = malloc(sizeof(int));
    *e = i;
    al_add(&al, e);
  }
  // insert 20 zeros.
  int* es[20];
  for (int i = 0; i < 20; i++) {
    int* e = malloc(sizeof(int));
    *e = 0;
    es[i] = e;
  }
  al_add_all(&al, (void**) es, 20);
  ck_assert_int_eq(al.cap, 64);
  ck_assert_int_eq(al.size, 40);
  int expected[] = {
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, \
    10, 11, 12, 13, 14, 15, 16, 17, 18, 19, \
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0, \
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0  \
  };
  for (int i = 0; i < al.size; i++) {
    int e = *((int*) al.data[i]);
    ck_assert_int_eq(e, expected[i]);
  }
} END_TEST

START_TEST(test_al_remove_at) {
  arraylist al;
  al_init(&al);

  int* es[10];
  for (int i = 0; i < 10; i++) {
    int* e = malloc(sizeof(int));
    *e = i;
    es[i] = e;
  }
  al_add_all(&al, (void**) es, 10);

  int* removed = (int*) al_remove_at(&al, 5);
  ck_assert_int_eq(*removed, 5);

  ck_assert_int_eq(al.cap, 16);
  ck_assert_int_eq(al.size, 9);
  int expected[] = { 0, 1, 2, 3, 4, 6, 7, 8, 9 };
  for (int i = 0; i < al.size; i++) {
    int e = *((int*) al.data[i]);
    ck_assert_int_eq(e, expected[i]);
  }
} END_TEST

START_TEST(test_al_remove_all_at) {
  arraylist al;
  al_init(&al);

  int* es[10];
  for (int i = 0; i < 10; i++) {
    int* e = malloc(sizeof(int));
    *e = i;
    es[i] = e;
  }
  al_add_all(&al, (void**) es, 10);

  int* removed[5];
  al_remove_all_at(&al, (void**) &removed, 3, 8);

  int expected_removed[] = { 3, 4, 5, 6, 7 };
  for (int i = 0; i < 5; i++) {
    ck_assert_int_eq(*removed[i], expected_removed[i]);
  }

  int expected_remaining[] = { 0, 1, 2, 8, 9 };
  for (int i = 0; i < 5; i++) {
    int e = *((int*) al.data[i]);
    ck_assert_int_eq(e, expected_remaining[i]);
  }
} END_TEST

START_TEST(test_al_add_at_outofbounds) {
  arraylist al;
  al_init(&al);

  for (int i = 0; i < 20; i++) {
    int* e = malloc(sizeof(int));
    *e = i;
    al_add_at(&al, e, 0);
  }
  // add out of bounds
  int* e = malloc(sizeof(int));
  *e = 20;
  bool result = al_add_at(&al, e, 21);
  ck_assert_int_eq(result, false);
  ck_assert_int_eq(al.cap, 32);
  ck_assert_int_eq(al.size, 20);
  for (int i = 0; i < al.size; i++) {
    ck_assert_int_eq(*((int*) al.data[al.size - 1 - i]), i);
  }
} END_TEST

START_TEST(test_al_add_all_at_outofbounds) {
  arraylist al;
  al_init(&al);

  for (int i = 0; i < 20; i++) {
    int* e = malloc(sizeof(int));
    *e = i;
    al_add(&al, e);
  }
  // insert 20 zeros out of bounds.
  int* es[20];
  for (int i = 0; i < 20; i++) {
    int* e = malloc(sizeof(int));
    *e = 0;
    es[i] = e;
  }
  bool result = al_add_all_at(&al, (void**) es, 20, 21);
  ck_assert_int_eq(result, false);
  ck_assert_int_eq(al.cap, 32);
  ck_assert_int_eq(al.size, 20);
  int expected[] = {
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, \
    10, 11, 12, 13, 14, 15, 16, 17, 18, 19  \
  };
  for (int i = 0; i < al.size; i++) {
    int e = *((int*) al.data[i]);
    ck_assert_int_eq(e, expected[i]);
  }
} END_TEST

START_TEST(test_al_remove_at_outofbounds) {
  arraylist al;
  al_init(&al);

  int* es[10];
  for (int i = 0; i < 10; i++) {
    int* e = malloc(sizeof(int));
    *e = i;
    es[i] = e;
  }
  al_add_all(&al, (void**) es, 10);

  int* removed = (int*) al_remove_at(&al, 11);
  ck_assert_ptr_null(removed);

  ck_assert_int_eq(al.cap, 16);
  ck_assert_int_eq(al.size, 10);
  int expected[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  for (int i = 0; i < al.size; i++) {
    int e = *((int*) al.data[i]);
    ck_assert_int_eq(e, expected[i]);
  }
} END_TEST

START_TEST(test_al_remove_all_at_outofbounds) {
  arraylist al;
  al_init(&al);

  int* es[10];
  for (int i = 0; i < 10; i++) {
    int* e = malloc(sizeof(int));
    *e = i;
    es[i] = e;
  }
  al_add_all(&al, (void**) es, 10);

  int* removed[5] = { NULL };
  al_remove_all_at(&al, (void**) &removed, 3, 11);

  for (int i = 0; i < 5; i++) {
    ck_assert_ptr_null(removed[i]);
  }

  int expected_remaining[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  for (int i = 0; i < 10; i++) {
    int e = *((int*) al.data[i]);
    ck_assert_int_eq(e, expected_remaining[i]);
  }
} END_TEST

START_TEST(test_al_remove_all_at_invalid_from_to) {
  arraylist al;
  al_init(&al);

  int* es[10];
  for (int i = 0; i < 10; i++) {
    int* e = malloc(sizeof(int));
    *e = i;
    es[i] = e;
  }
  al_add_all(&al, (void**) es, 10);

  int* removed[5] = { NULL };
  al_remove_all_at(&al, (void**) &removed, 8, 3);

  for (int i = 0; i < 5; i++) {
    ck_assert_ptr_null(removed[i]);
  }

  int expected_remaining[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  for (int i = 0; i < 10; i++) {
    int e = *((int*) al.data[i]);
    ck_assert_int_eq(e, expected_remaining[i]);
  }
} END_TEST

Suite* arraylist_suite(void) {
  Suite *s;
  TCase *tc_add;
  TCase *tc_remove;
  TCase *tc_outofbounds;

  s = suite_create("arraylist_suite");
  tc_add = tcase_create("add");
  tc_remove = tcase_create("remove");
  tc_outofbounds = tcase_create("outofbounds");

  tcase_add_test(tc_add, test_al_init);
  tcase_add_test(tc_add, test_al_add_at_expand);
  tcase_add_test(tc_add, test_al_add_expand);
  tcase_add_test(tc_add, test_al_add_all_at_expand);
  tcase_add_test(tc_add, test_al_add_all_at_empty);
  tcase_add_test(tc_add, test_al_add_all_expand);
  suite_add_tcase(s, tc_add);

  tcase_add_test(tc_remove, test_al_remove_at);
  tcase_add_test(tc_remove, test_al_remove_all_at);
  suite_add_tcase(s, tc_remove);

  tcase_add_test(tc_outofbounds, test_al_add_at_outofbounds);
  tcase_add_test(tc_outofbounds, test_al_add_all_at_outofbounds);
  tcase_add_test(tc_outofbounds, test_al_remove_at_outofbounds);
  tcase_add_test(tc_outofbounds, test_al_remove_all_at_outofbounds);
  tcase_add_test(tc_outofbounds, test_al_remove_all_at_invalid_from_to);
  suite_add_tcase(s, tc_outofbounds);

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

