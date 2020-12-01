#include <check.h>
#include <stdlib.h>

#include <sequence.h>

START_TEST(test_seq_insert) {
  Sequence *s = seq_new();
  char *data = "this is a string";
  int n = strlen(data);
  for (int i = 0; i < n; i++) {
    seq_insert(s, data + i, i);
  }

  // check that all elements are stored properly.
  for (int i = 0; i < n; i++) {
    Element *e = seq_get_element(s, i);
    char c = *((char*) e->data.ptr);
    ck_assert_int_eq(c, data[i]);
  }

  // check that all elements are sorted by Guid.
  for (unsigned int i = 1; i < s->elements.size; i++) {
    Element *prev = al_get(&s->elements, i - 1);
    Element *curr = al_get(&s->elements, i);
    ck_assert_int_lt(guid_compare(&prev->id, &curr->id), 0);
  }
} END_TEST

START_TEST(test_seq_insert_save) {
  Sequence *s = seq_new();
  char *data = "this is a string";
  int n = strlen(data);
  for (int i = 0; i < n; i++) {
    Element e;
    // insert and save the element inserted.
    seq_insert_save(s, data + i, i, &e);
    // check if the data pointer is the same.
    ck_assert_ptr_eq(data + i, e.data.ptr);
  }

  // check that all elements are stored properly.
  for (int i = 0; i < n; i++) {
    Element *e = seq_get_element(s, i);
    char c = *((char*) e->data.ptr);
    ck_assert_int_eq(c, data[i]);
  }

  // check that all elements are sorted by Guid.
  for (unsigned int i = 1; i < s->elements.size; i++) {
    Element *prev = al_get(&s->elements, i - 1);
    Element *curr = al_get(&s->elements, i);
    ck_assert_int_lt(guid_compare(&prev->id, &curr->id), 0);
  }
} END_TEST

START_TEST(test_seq_insert_value) {
  Sequence *s = seq_new();
  char *data = "this is a string";
  int n = strlen(data);
  for (int i = 0; i < n; i++) {
    seq_insert_value(s, data[i], i);
  }

  // check that all elements are stored properly.
  for (int i = 0; i < n; i++) {
    Element *e = seq_get_element(s, i);
    char c = e->data.value;
    ck_assert_int_eq(c, data[i]);
  }

  // check that all elements are sorted by Guid.
  for (unsigned int i = 1; i < s->elements.size; i++) {
    Element *prev = al_get(&s->elements, i - 1);
    Element *curr = al_get(&s->elements, i);
    ck_assert_int_lt(guid_compare(&prev->id, &curr->id), 0);
  }
} END_TEST

START_TEST(test_seq_delete) {
  Sequence *s = seq_new();
  char *data = "this is a string";
  int n = strlen(data);
  for (int i = 0; i < n; i++) {
    seq_insert_value(s, data[i], i);
  }

  // delete 'h', then delete 'is'
  seq_delete(s, 1);
  seq_delete(s, 4);
  seq_delete(s, 4);

  char *expected = "tis  a string";
  n = strlen(expected);
  // check that all elements are stored properly.
  for (int i = 0; i < n; i++) {
    Element *e = seq_get_element(s, i);
    char c = e->data.value;
    ck_assert_int_eq(c, expected[i]);
  }

  // check that all elements are sorted by Guid.
  for (unsigned int i = 1; i < s->elements.size; i++) {
    Element *prev = al_get(&s->elements, i - 1);
    Element *curr = al_get(&s->elements, i);
    ck_assert_int_lt(guid_compare(&prev->id, &curr->id), 0);
  }
} END_TEST

START_TEST(test_seq_remote_insert) {
  Sequence *s = seq_new();
  char *data = "this is a string";
  int n = strlen(data);
  Element remote_inserts[n];
  for (int i = 0; i < n; i++) {
    seq_insert_value_save(s, data[i], i, &remote_inserts[i]);
  }

  Sequence *s2 = seq_new();
  // insert remote elements twice.
  for (int i = 0; i < n; i++) {
    seq_remote_insert(s2, &remote_inserts[i]);
  }
  for (int i = 0; i < n; i++) {
    seq_remote_insert(s2, &remote_inserts[i]);
  }

  // check that all elements are stored properly.
  ck_assert_int_eq(n, seq_size(s2));
  for (int i = 0; i < n; i++) {
    Element *e2 = seq_get_element(s2, i);
    char c2 = e2->data.value;
    ck_assert_int_eq(c2, data[i]);
  }

  // check that all elements are sorted by Guid.
  for (unsigned int i = 1; i < seq_size(s2); i++) {
    Element *prev = seq_get_element(s2, i - 1);
    Element *curr = seq_get_element(s2, i);
    ck_assert_int_lt(guid_compare(&prev->id, &curr->id), 0);
  }

} END_TEST

START_TEST(test_seq_remote_delete) {
  Sequence *s = seq_new();
  Sequence *s2 = seq_new();

  char *data = "this is a string";
  int n = strlen(data);
  for (int i = 0; i < n; i++) {
    Element buf;
    seq_insert_value_save(s, data[i], i, &buf);
    seq_remote_insert(s2, &buf);
  }

  Element remote_deletes[3];
  // delete 'h', then delete 'is'
  seq_delete_save(s, 1, &remote_deletes[0]);
  seq_delete_save(s, 4, &remote_deletes[1]);
  seq_delete_save(s, 4, &remote_deletes[2]);

  // apply remote deletes not in order.
  for (int i = 2; i >= 0; i--) {
    seq_remote_delete(s2, &remote_deletes[i]);
  }

  char *expected = "tis  a string";
  n = strlen(expected);
  // check that all elements are stored properly.
  for (int i = 0; i < n; i++) {
    Element *e = seq_get_element(s2, i);
    char c = e->data.value;
    ck_assert_int_eq(c, expected[i]);
  }

  // check that all elements are sorted by Guid.
  for (unsigned int i = 1; i < s2->elements.size; i++) {
    Element *prev = al_get(&s->elements, i - 1);
    Element *curr = al_get(&s->elements, i);
    ck_assert_int_lt(guid_compare(&prev->id, &curr->id), 0);
  }
} END_TEST

START_TEST(test_seq_insert_out_of_bounds) {
  Sequence *s = seq_new();
  char *data = "this is a string";
  int n = strlen(data);
  for (int i = 0; i < n; i++) {
    seq_insert(s, data + i, i);
  }

  // n + 1 is out of bounds, but n is in bounds.
  ck_assert(seq_insert_value(s, 'a', n + 1) == false);
  ck_assert(seq_insert_value(s, 'a', n) == true);
} END_TEST

Suite *sequence_insert_delete_suite(void) {
  Suite *s;
  TCase *tc_core;
  TCase *tc_boundary;

  s = suite_create("check_sequence_insert_delete");
  tc_core = tcase_create("core");
  tc_boundary = tcase_create("boundary");

  tcase_add_test(tc_core, test_seq_insert);
  tcase_add_test(tc_core, test_seq_insert_save);
  tcase_add_test(tc_core, test_seq_insert_value);
  tcase_add_test(tc_core, test_seq_delete);
  tcase_add_test(tc_core, test_seq_remote_insert);
  tcase_add_test(tc_core, test_seq_remote_delete);
  suite_add_tcase(s, tc_core);

  tcase_add_test(tc_boundary, test_seq_insert_out_of_bounds);
  suite_add_tcase(s, tc_boundary);

  return s;
}

