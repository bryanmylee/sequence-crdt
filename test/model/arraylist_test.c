#include <check.h>
#include <stdlib.h>
#include <model/arraylist.h>

START_TEST(test_al_init) {
  ArrayList al;
  al_init(&al, sizeof(int));
  ck_assert_int_eq(al.cap, 16);
  ck_assert_int_eq(al.size, 0);
  al_free_internal(&al);
} END_TEST

START_TEST(test_al_new) {
  ArrayList* al = al_new(sizeof(int));
  ck_assert_int_eq(al->cap, 16);
  ck_assert_int_eq(al->size, 0);
  free(al);
} END_TEST

START_TEST(test_al_add_at_expand) {
  ArrayList al;
  al_init(&al, sizeof(int));

  for (int i = 0; i < 20; i++) {
    al_add_at(&al, &i, 0);
  }

  ck_assert_int_eq(al.cap, 32);
  ck_assert_int_eq(al.size, 20);
  for (int i = 0; i < al.size; i++) {
    int* e = al_get(&al, al.size - 1 - i);
    ck_assert_int_eq(*e, i);
  }
  al_free_internal(&al);
} END_TEST

START_TEST(test_al_add_expand) {
  ArrayList al;
  al_init(&al, sizeof(int));

  for (int i = 0; i < 20; i++) {
    al_add(&al, &i);
  }

  ck_assert_int_eq(al.cap, 32);
  ck_assert_int_eq(al.size, 20);
  for (int i = 0; i < al.size; i++) {
    int* e = al_get(&al, i);
    ck_assert_int_eq(*e, i);
  }
  al_free_internal(&al);
} END_TEST

START_TEST(test_al_add_all_at_expand) {
  ArrayList al;
  al_init(&al, sizeof(int));
  for (int i = 0; i < 20; i++) {
    al_add(&al, &i);
  }

  // insert 20 zeros at index 10.
  int es[20] = { 0 };
  al_add_all_at(&al, es, 20, 10);

  ck_assert_int_eq(al.cap, 64);
  ck_assert_int_eq(al.size, 40);
  int expected[] = {
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, \
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0, \
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0, \
    10, 11, 12, 13, 14, 15, 16, 17, 18, 19  \
  };
  for (int i = 0; i < al.size; i++) {
    int* e = al_get(&al, i);
    ck_assert_int_eq(*e, expected[i]);
  }
  al_free_internal(&al);
} END_TEST

START_TEST(test_al_add_all_at_empty) {
  ArrayList al;
  al_init(&al, sizeof(int));

  // insert 20 zeros at index 0.
  int es[20] = { 0 };
  al_add_all_at(&al, es, 20, 0);

  ck_assert_int_eq(al.cap, 32);
  ck_assert_int_eq(al.size, 20);
  int expected[] = {
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0, \
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0  \
  };
  for (int i = 0; i < al.size; i++) {
    int* e = al_get(&al, i);
    ck_assert_int_eq(*e, expected[i]);
  }
  al_free_internal(&al);
} END_TEST

START_TEST(test_al_add_all_expand) {
  ArrayList al;
  al_init(&al, sizeof(int));
  for (int i = 0; i < 20; i++) {
    al_add(&al, &i);
  }

  // insert 20 zeros.
  int es[20] = { 0 };
  al_add_all(&al, es, 20);

  ck_assert_int_eq(al.cap, 64);
  ck_assert_int_eq(al.size, 40);
  int expected[] = {
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, \
    10, 11, 12, 13, 14, 15, 16, 17, 18, 19, \
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0, \
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0  \
  };
  for (int i = 0; i < al.size; i++) {
    int* e = al_get(&al, i);
    ck_assert_int_eq(*e, expected[i]);
  }
  al_free_internal(&al);
} END_TEST

START_TEST(test_al_remove_at) {
  ArrayList al;
  al_init(&al, sizeof(int));
  int es[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  al_add_all(&al, es, 10);

  int removed;
  al_remove_at_save(&al, 5, &removed);
  ck_assert_int_eq(removed, 5);
  al_remove_at_save(&al, 5, &removed);
  ck_assert_int_eq(removed, 6);

  ck_assert_int_eq(al.cap, 16);
  ck_assert_int_eq(al.size, 8);
  int expected[] = { 0, 1, 2, 3, 4, 7, 8, 9 };
  for (int i = 0; i < al.size; i++) {
    int* e = al_get(&al, i);
    ck_assert_int_eq(*e, expected[i]);
  }
  al_free_internal(&al);
} END_TEST

START_TEST(test_al_remove_all_at) {
  ArrayList al;
  al_init(&al, sizeof(int));

  int es[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  al_add_all(&al, es, 10);

  int removed[5];
  al_remove_all_at_save(&al, 3, 8, removed);

  int expected_removed[] = { 3, 4, 5, 6, 7 };
  for (int i = 0; i < 5; i++) {
    ck_assert_int_eq(removed[i], expected_removed[i]);
  }

  int expected_remaining[] = { 0, 1, 2, 8, 9 };
  for (int i = 0; i < 5; i++) {
    int* e = al_get(&al, i);
    ck_assert_int_eq(*e, expected_remaining[i]);
  }
  al_free_internal(&al);
} END_TEST

START_TEST(test_al_add_at_boundary) {
  ArrayList al;
  al_init(&al, sizeof(int));
  for (int i = 19; i >= 0; i--) {
    al_add_at(&al, &i, 0);
  }

  int e = 20;
  bool result = al_add_at(&al, &e, 20);

  ck_assert_int_eq(result, true);
  ck_assert_int_eq(al.cap, 32);
  ck_assert_int_eq(al.size, 21);
  for (int i = 0; i < al.size; i++) {
    int* e = al_get(&al, i);
    ck_assert_int_eq(*e, i);
  }
  al_free_internal(&al);
} END_TEST

START_TEST(test_al_add_past_boundary) {
  ArrayList al;
  al_init(&al, sizeof(int));
  for (int i = 19; i >= 0; i--) {
    al_add_at(&al, &i, 0);
  }

  int e = 20;
  bool result = al_add_at(&al, &e, 21);

  ck_assert_int_eq(result, false);
  ck_assert_int_eq(al.cap, 32);
  ck_assert_int_eq(al.size, 20);
  for (int i = 0; i < al.size; i++) {
    int* e = al_get(&al, i);
    ck_assert_int_eq(*e, i);
  }
  al_free_internal(&al);
} END_TEST

START_TEST(test_al_add_all_at_boundary) {
  ArrayList al;
  al_init(&al, sizeof(int));
  for (int i = 0; i < 20; i++) {
    al_add(&al, &i);
  }
  // insert 20 zeros out of bounds.
  int es[20] = { 0 };
  bool result = al_add_all_at(&al, es, 20, 20);

  ck_assert_int_eq(result, true);
  ck_assert_int_eq(al.cap, 64);
  ck_assert_int_eq(al.size, 40);
  int expected[] = {
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, \
    10, 11, 12, 13, 14, 15, 16, 17, 18, 19, \
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0, \
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0, \
  };
  for (int i = 0; i < al.size; i++) {
    int* e = al_get(&al, i);
    ck_assert_int_eq(*e, expected[i]);
  }
  al_free_internal(&al);
} END_TEST

START_TEST(test_al_add_all_past_boundary) {
  ArrayList al;
  al_init(&al, sizeof(int));
  for (int i = 0; i < 20; i++) {
    al_add(&al, &i);
  }
  // insert 20 zeros out of bounds.
  int es[20] = { 0 };
  bool result = al_add_all_at(&al, es, 20, 21);

  ck_assert_int_eq(result, false);
  ck_assert_int_eq(al.cap, 32);
  ck_assert_int_eq(al.size, 20);
  int expected[] = {
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, \
    10, 11, 12, 13, 14, 15, 16, 17, 18, 19  \
  };
  for (int i = 0; i < al.size; i++) {
    int* e = al_get(&al, i);
    ck_assert_int_eq(*e, expected[i]);
  }
  al_free_internal(&al);
} END_TEST

START_TEST(test_al_remove_at_boundary) {
  ArrayList al;
  al_init(&al, sizeof(int));
  int es[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  al_add_all(&al, es, 10);

  int removed = 0;
  al_remove_at_save(&al, 10, &removed);
  ck_assert_int_eq(removed, 0);

  ck_assert_int_eq(al.cap, 16);
  ck_assert_int_eq(al.size, 10);
  int expected[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  for (int i = 0; i < al.size; i++) {
    int* e = al_get(&al, i);
    ck_assert_int_eq(*e, expected[i]);
  }
  al_free_internal(&al);
} END_TEST

START_TEST(test_al_remove_past_boundary) {
  ArrayList al;
  al_init(&al, sizeof(int));
  int es[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  al_add_all(&al, es, 10);

  int removed = 0;
  al_remove_at_save(&al, 11, &removed);
  ck_assert_int_eq(removed, 0);

  ck_assert_int_eq(al.cap, 16);
  ck_assert_int_eq(al.size, 10);
  int expected[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  for (int i = 0; i < al.size; i++) {
    int* e = al_get(&al, i);
    ck_assert_int_eq(*e, expected[i]);
  }
  al_free_internal(&al);
} END_TEST

START_TEST(test_al_remove_all_at_boundary) {
  ArrayList al;
  al_init(&al, sizeof(int));
  int es[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  al_add_all(&al, es, 10);

  int removed[8] = { 0 };
  al_remove_all_at_save(&al, 2, 10, removed);

  for (int i = 0; i < 8; i++) {
    ck_assert_int_eq(removed[i], 2 + i);
  }
  int expected_remaining[] = { 0, 1 };
  for (int i = 0; i < 2; i++) {
    int* e = al_get(&al, i);
    ck_assert_int_eq(*e, expected_remaining[i]);
  }
  al_free_internal(&al);
} END_TEST

START_TEST(test_al_remove_all_past_boundary) {
  ArrayList al;
  al_init(&al, sizeof(int));
  int es[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  al_add_all(&al, es, 10);

  int removed[5] = { 0 };
  al_remove_all_at_save(&al, 3, 11, removed);

  for (int i = 0; i < 5; i++) {
    ck_assert_int_eq(removed[i], 0);
  }
  int expected_remaining[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  for (int i = 0; i < 10; i++) {
    int* e = al_get(&al, i);
    ck_assert_int_eq(*e, expected_remaining[i]);
  }
  al_free_internal(&al);
} END_TEST

START_TEST(test_al_remove_all_at_invalid_from_to) {
  ArrayList al;
  al_init(&al, sizeof(int));

  int es[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  al_add_all(&al, es, 10);

  int removed[5] = { 0 };
  al_remove_all_at_save(&al, 8, 3, removed);

  for (int i = 0; i < 5; i++) {
    ck_assert_int_eq(removed[i], 0);
  }
  int expected_remaining[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  for (int i = 0; i < 10; i++) {
    int* e = al_get(&al, i);
    ck_assert_int_eq(*e, expected_remaining[i]);
  }
  al_free_internal(&al);
} END_TEST

Suite* arraylist_suite(void) {
  Suite *s;
  TCase *tc_add;
  TCase *tc_remove;
  TCase *tc_boundary;

  s = suite_create("arraylist_suite");
  tc_add = tcase_create("add");
  tc_remove = tcase_create("remove");
  tc_boundary = tcase_create("boundary");

  tcase_add_test(tc_add, test_al_init);
  tcase_add_test(tc_add, test_al_new);
  tcase_add_test(tc_add, test_al_add_at_expand);
  tcase_add_test(tc_add, test_al_add_expand);
  tcase_add_test(tc_add, test_al_add_all_at_expand);
  tcase_add_test(tc_add, test_al_add_all_at_empty);
  tcase_add_test(tc_add, test_al_add_all_expand);
  suite_add_tcase(s, tc_add);

  tcase_add_test(tc_remove, test_al_remove_at);
  tcase_add_test(tc_remove, test_al_remove_all_at);
  suite_add_tcase(s, tc_remove);

  tcase_add_test(tc_boundary, test_al_add_at_boundary);
  tcase_add_test(tc_boundary, test_al_add_past_boundary);
  tcase_add_test(tc_boundary, test_al_add_all_at_boundary);
  tcase_add_test(tc_boundary, test_al_add_all_past_boundary);
  tcase_add_test(tc_boundary, test_al_remove_at_boundary);
  tcase_add_test(tc_boundary, test_al_remove_past_boundary);
  tcase_add_test(tc_boundary, test_al_remove_all_at_boundary);
  tcase_add_test(tc_boundary, test_al_remove_all_past_boundary);
  tcase_add_test(tc_boundary, test_al_remove_all_at_invalid_from_to);
  suite_add_tcase(s, tc_boundary);

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

