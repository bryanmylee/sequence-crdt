#include <check.h>
#include <stdlib.h>
#include <model/arraylist.h>
#include <model/crdt/element.h>
#include <model/crdt/guid.h>
#include <model/crdt/sequence.h>

START_TEST(test_seq_gen_token_between) {
  token l = { .key = 4, .uid = 1 };
  token r = { .key = 6, .uid = 1 };
  token expected = { .key = 5, .uid = 1 };
  token result = seq_gen_token_between(&l, &r, 3, 1);
  ck_assert_uint_eq(result.key, expected.key);
  ck_assert_int_eq(result.uid, expected.uid);
} END_TEST

START_TEST(test_seq_gen_token_between_no_space) {
  token l = { .key = 4, .uid = 1 };
  token r = { .key = 5, .uid = 1 };
  token expected = { .key = -1, .uid = -1 };
  token result = seq_gen_token_between(&l, &r, 3, 1);
  ck_assert_uint_eq(result.key, expected.key);
  ck_assert_int_eq(result.uid, expected.uid);
} END_TEST

START_TEST(test_seq_gen_guid_between_siblings) {
  Guid l = {
    .depth = 3,
    .keys = keys_from_tokens(3, 1, 2, 1),
    .uids = uids_from_tokens(3, 1, 1, 1),
  };
  Guid r = {
    .depth = 3,
    .keys = keys_from_tokens(3, 1, 2, 3),
    .uids = uids_from_tokens(3, 1, 1, 1),
  };
  Guid expected = {
    .depth = 3,
    .keys = keys_from_tokens(3, 1, 2, 2),
    .uids = uids_from_tokens(3, 1, 1, 2),
  };
  Guid result;
  seq_gen_guid_between(&result, &l, &r, 2);
  ck_assert_int_eq(result.depth, expected.depth);
  ck_assert_uint_eq(result.keys, expected.keys);
  ck_assert_int_eq(result.uids, expected.uids);
} END_TEST

START_TEST(test_seq_gen_guid_between_siblings_no_space) {
  Guid l = {
    .depth = 3,
    .keys = keys_from_tokens(3, 1, 2, 1),
    .uids = uids_from_tokens(3, 1, 1, 1),
  };
  Guid r = {
    .depth = 3,
    .keys = keys_from_tokens(3, 1, 2, 2),
    .uids = uids_from_tokens(3, 1, 1, 1),
  };
  Guid result;
  seq_gen_guid_between(&result, &l, &r, 2);
  // next Guid is one level deeper.
  Guid expected = {
    .depth = 4,
    .uids = uids_from_tokens(4, 1, 1, 1, 2),
  };
  int base = 1 << 4;
  ck_assert_int_eq(result.depth, expected.depth);
  ck_assert_uint_ge(result.keys, keys_from_tokens(4, 1, 2, 2, 0));
  ck_assert_uint_lt(result.keys, keys_from_tokens(4, 1, 2, 2, base));
  ck_assert_int_eq(result.uids, expected.uids);
} END_TEST

START_TEST(test_seq_gen_guid_between_parent_child) {
  Guid l = {
    .depth = 3,
    .keys = keys_from_tokens(3, 0, 1, 2),
    .uids = uids_from_tokens(3, 1, 1, 1),
  };
  Guid r = {
    .depth = 4,
    .keys = keys_from_tokens(4, 0, 1, 2, 2),
    .uids = uids_from_tokens(4, 1, 1, 1, 1),
  };
  Guid result;
  seq_gen_guid_between(&result, &l, &r, 1);
  ck_assert_int_lt(guid_compare(&l, &result), 0);
  ck_assert_int_lt(guid_compare(&result, &r), 0);
} END_TEST

START_TEST(test_seq_gen_guid_between_uncle_nephew) {
  Guid l = {
    .depth = 3,
    .keys = keys_from_tokens(3, 0, 1, 2),
    .uids = uids_from_tokens(3, 1, 1, 1),
  };
  Guid r = {
    .depth = 4,
    .keys = keys_from_tokens(4, 1, 1, 2, 2),
    .uids = uids_from_tokens(4, 1, 1, 1, 1),
  };
  Guid result;
  seq_gen_guid_between(&result, &l, &r, 1);
  ck_assert_int_lt(guid_compare(&l, &result), 0);
  ck_assert_int_lt(guid_compare(&result, &r), 0);
} END_TEST

START_TEST(test_seq_gen_guid_between_nephew_uncle) {
  Guid l = {
    .depth = 4,
    .keys = keys_from_tokens(4, 0, 1, 2, 2),
    .uids = uids_from_tokens(4, 1, 1, 1, 1),
  };
  Guid r = {
    .depth = 3,
    .keys = keys_from_tokens(3, 0, 2, 2),
    .uids = uids_from_tokens(3, 1, 1, 1),
  };
  Guid result;
  seq_gen_guid_between(&result, &l, &r, 1);
  ck_assert_int_lt(guid_compare(&l, &result), 0);
  ck_assert_int_lt(guid_compare(&result, &r), 0);
} END_TEST

START_TEST(test_seq_gen_guid_between_same_key_different_uid) {
  Guid l = {
    .depth = 3,
    .keys = keys_from_tokens(3, 0, 2, 2),
    .uids = uids_from_tokens(3, 1, 1, 1),
  };
  Guid r = {
    .depth = 3,
    .keys = keys_from_tokens(3, 0, 2, 2),
    .uids = uids_from_tokens(3, 1, 2, 1),
  };
  Guid result;
  seq_gen_guid_between(&result, &l, &r, 1);
  ck_assert_int_lt(guid_compare(&l, &result), 0);
  ck_assert_int_lt(guid_compare(&result, &r), 0);
} END_TEST

START_TEST(test_seq_iindex_of_even) {
  Sequence* s = seq_new();
  // remove header and trailer elements for the purpose of this test.
  al_init(&s->elements, sizeof(Element));
  // insert 8 Element pointers.
  for (int i = 0; i < 8; i++) {
    Element* e = element_new();
    e->value = malloc(sizeof(int));
    // give all elements a simple incremental guid.
    // at depth 3, all 8 elements can fit under one node.
    e->id = (Guid) {
      .depth = 3,
      .keys = keys_from_tokens(3, 0, 0, i),
    };
    *((int*) e->value) = i;
    al_add(&s->elements, e);
  }

  // somewhere in the middle.
  Element to_find;
  element_init(&to_find);
  to_find.id = (Guid) {
    .depth = 3,
    .keys = keys_from_tokens(3, 0, 0, 5),
  };
  int iindex_of_to_find = seq_iindex_of_element_or_after(s, &to_find);
  ck_assert_int_eq(iindex_of_to_find, 5);

  // at the start.
  to_find.id = (Guid) {
    .depth = 3,
    .keys = keys_from_tokens(3, 0, 0, 0),
  };
  iindex_of_to_find = seq_iindex_of_element_or_after(s, &to_find);
  ck_assert_int_eq(iindex_of_to_find, 0);

  // at the end.
  to_find.id = (Guid) {
    .depth = 3,
    .keys = keys_from_tokens(3, 0, 0, 7),
  };
  iindex_of_to_find = seq_iindex_of_element_or_after(s, &to_find);
  ck_assert_int_eq(iindex_of_to_find, 7);

  seq_free(&s);
} END_TEST

START_TEST(test_seq_iindex_of_odd) {
  Sequence* s = seq_new();
  // remove header and trailer elements for the purpose of this test.
  al_init(&s->elements, sizeof(Element));
  // insert 7 Element pointers.
  for (int i = 0; i < 7; i++) {
    Element* e = element_new();
    e->value = malloc(sizeof(int));
    // give all elements a simple incremental guid.
    // at depth 3, all 8 elements can fit under one node.
    e->id = (Guid) {
      .depth = 3,
      .keys = keys_from_tokens(3, 0, 0, i),
    };
    *((int*) e->value) = i;
    al_add(&s->elements, e);
  }

  // somewhere in the middle.
  Element to_find;
  element_init(&to_find);
  to_find.id = (Guid) {
    .depth = 3,
    .keys = keys_from_tokens(3, 0, 0, 4),
  };
  int iindex_of_to_find = seq_iindex_of_element_or_after(s, &to_find);
  ck_assert_int_eq(iindex_of_to_find, 4);

  // at the start.
  to_find.id = (Guid) {
    .depth = 3,
    .keys = keys_from_tokens(3, 0, 0, 0),
  };
  iindex_of_to_find = seq_iindex_of_element_or_after(s, &to_find);
  ck_assert_int_eq(iindex_of_to_find, 0);

  // at the end.
  to_find.id = (Guid) {
    .depth = 3,
    .keys = keys_from_tokens(3, 0, 0, 6),
  };
  iindex_of_to_find = seq_iindex_of_element_or_after(s, &to_find);
  ck_assert_int_eq(iindex_of_to_find, 6);

  seq_free(&s);
} END_TEST

START_TEST(test_seq_iindex_of_even_non_existent) {
  Sequence* s = seq_new();
  // remove header and trailer elements for the purpose of this test.
  al_init(&s->elements, sizeof(Element));
  // insert 8 Element pointers with even key tokens.
  for (int i = 0; i < 8; i++) {
    Element* e = element_new();
    e->value = malloc(sizeof(int));
    // give all elements a simple incremental guid.
    // at depth 4, there are 16 possible locations.
    e->id = (Guid) {
      .depth = 4,
      .keys = keys_from_tokens(4, 0, 0, 0, i * 2),
    };
    *((int*) e->value) = i;
    al_add(&s->elements, e);
  }

  // somewhere in the middle.
  Element to_find;
  element_init(&to_find);
  to_find.id = (Guid) {
    .depth = 4,
    .keys = keys_from_tokens(4, 0, 0, 0, 5),
  };
  int iindex_of_to_find = seq_iindex_of_element_or_after(s, &to_find);
  // index of element with token 6 is 3.
  ck_assert_int_eq(iindex_of_to_find, 3);

  // at the start, before index 0.
  element_init(&to_find);
  to_find.id = (Guid) {
    .depth = 3,
    .keys = keys_from_tokens(3, 0, 0, 0),
  };
  iindex_of_to_find = seq_iindex_of_element_or_after(s, &to_find);
  ck_assert_int_eq(iindex_of_to_find, 0);

  // at the end.
  element_init(&to_find);
  to_find.id = (Guid) {
    .depth = 3,
    .keys = keys_from_tokens(3, 1, 0, 0),
  };
  iindex_of_to_find = seq_iindex_of_element_or_after(s, &to_find);
  // element will come after the last element.
  ck_assert_int_eq(iindex_of_to_find, 8);

  seq_free(&s);
} END_TEST

START_TEST(test_seq_iindex_of_odd_non_existent) {
  Sequence* s = seq_new();
  // remove header and trailer elements for the purpose of this test.
  al_init(&s->elements, sizeof(Element));
  // insert 7 Element pointers with even key tokens.
  for (int i = 0; i < 7; i++) {
    Element* e = element_new();
    e->value = malloc(sizeof(int));
    // give all elements a simple incremental guid.
    // at depth 4, there are 16 possible locations.
    e->id = (Guid) {
      .depth = 4,
      .keys = keys_from_tokens(4, 0, 0, 0, i * 2),
    };
    *((int*) e->value) = i;
    al_add(&s->elements, e);
  }

  // somewhere in the middle.
  Element to_find;
  element_init(&to_find);
  to_find.id = (Guid) {
    .depth = 4,
    .keys = keys_from_tokens(4, 0, 0, 0, 5),
  };
  int iindex_of_to_find = seq_iindex_of_element_or_after(s, &to_find);
  // index of element with token 6 is 3.
  ck_assert_int_eq(iindex_of_to_find, 3);

  // at the start, before index 0.
  element_init(&to_find);
  to_find.id = (Guid) {
    .depth = 3,
    .keys = keys_from_tokens(3, 0, 0, 0),
  };
  iindex_of_to_find = seq_iindex_of_element_or_after(s, &to_find);
  ck_assert_int_eq(iindex_of_to_find, 0);

  // at the end.
  element_init(&to_find);
  to_find.id = (Guid) {
    .depth = 3,
    .keys = keys_from_tokens(3, 1, 0, 0),
  };
  iindex_of_to_find = seq_iindex_of_element_or_after(s, &to_find);
  // element will come after the last element.
  ck_assert_int_eq(iindex_of_to_find, 7);

  seq_free(&s);
} END_TEST

START_TEST(test_seq_insert) {
  Sequence* s = seq_new();
  char* data = "this is a string";
  int n = strlen(data);
  for (int i = 0; i < n; i++) {
    seq_insert(s, data + i, i);
  }

  // check that all elements are stored properly.
  for (int i = 0; i < n; i++) {
    Element* e = seq_get_element(s, i);
    char c = *((char*) e->value);
    ck_assert_int_eq(c, data[i]);
  }

  // check that all elements are sorted by Guid.
  for (int i = 1; i < s->elements.size; i++) {
    Element* prev = ((Element**) s->elements.data)[i - 1];
    Element* curr = ((Element**) s->elements.data)[i];
    ck_assert_int_lt(guid_compare(&prev->id, &curr->id), 0);
  }
}

START_TEST(test_seq_delete) {
  Sequence* s = seq_new();
  char* data = "this is a string";
  int n = strlen(data);
  for (int i = 0; i < n; i++) {
    seq_insert(s, data + i, i);
  }

  // delete 'h', then delete 'is'
  seq_delete(s, 1);
  seq_delete(s, 4);
  seq_delete(s, 4);

  char* expected = "tis  a string";
  n = strlen(expected);
  // check that all elements are stored properly.
  for (int i = 0; i < n; i++) {
    Element* e = seq_get_element(s, i);
    char c = *((char*) e->value);
    ck_assert_int_eq(c, expected[i]);
  }

  // check that all elements are sorted by Guid.
  for (int i = 1; i < s->elements.size; i++) {
    Element* prev = ((Element**) s->elements.data)[i - 1];
    Element* curr = ((Element**) s->elements.data)[i];
    ck_assert_int_lt(guid_compare(&prev->id, &curr->id), 0);
  }
}

START_TEST(test_seq_remote_insert) {
  Sequence* s = seq_new();
  char* data = "this is a string";
  int n = strlen(data);
  Element* remote_inserts[n];
  for (int i = 0; i < n; i++) {
    remote_inserts[i] = seq_insert(s, data + i, i);
  }

  Sequence* s2 = seq_new();
  // insert remote elements twice.
  for (int i = 0; i < n; i++) {
    seq_remote_insert(s2, remote_inserts[i]);
  }
  for (int i = 0; i < n; i++) {
    seq_remote_insert(s2, remote_inserts[i]);
  }

  // check that all elements are stored properly.
  for (int i = 0; i < n; i++) {
    Element* e = seq_get_element(s, i);
    char c = *((char*) e->value);
    ck_assert_int_eq(c, data[i]);
  }

  // check that all elements are sorted by Guid.
  for (int i = 1; i < s2->elements.size; i++) {
    Element* prev = ((Element**) s2->elements.data)[i - 1];
    Element* curr = ((Element**) s2->elements.data)[i];
    ck_assert_int_lt(guid_compare(&prev->id, &curr->id), 0);
  }
}

START_TEST(test_seq_remote_delete) {
  Sequence* s = seq_new();
  Sequence* s2 = seq_new();

  char* data = "this is a string";
  int n = strlen(data);
  for (int i = 0; i < n; i++) {
    seq_remote_insert(s2, seq_insert(s, data + i, i));
  }

  Element* remote_deletes[3];
  // delete 'h', then delete 'is'
  remote_deletes[0] = seq_delete(s, 1);
  remote_deletes[1] = seq_delete(s, 4);
  remote_deletes[2] = seq_delete(s, 4);

  // apply remote deletes not in order.
  for (int i = 2; i >= 0; i--) {
    seq_remote_delete(s2, remote_deletes[i]);
  }

  char* expected = "tis  a string";
  n = strlen(expected);
  // check that all elements are stored properly.
  for (int i = 0; i < n; i++) {
    Element* e = seq_get_element(s2, i);
    char c = *((char*) e->value);
    ck_assert_int_eq(c, expected[i]);
  }

  // check that all elements are sorted by Guid.
  for (int i = 1; i < s2->elements.size; i++) {
    Element* prev = ((Element**) s2->elements.data)[i - 1];
    Element* curr = ((Element**) s2->elements.data)[i];
    ck_assert_int_lt(guid_compare(&prev->id, &curr->id), 0);
  }
}

Suite* sequence_suite(void) {
  Suite *s;
  TCase *tc_guid;
  TCase *tc_find;
  TCase *tc_insert_delete;

  s = suite_create("sequence_suite");
  tc_guid = tcase_create("guid");
  tc_find = tcase_create("find");
  tc_insert_delete = tcase_create("insert delete");

  tcase_add_test(tc_guid, test_seq_gen_token_between);
  tcase_add_test(tc_guid, test_seq_gen_token_between_no_space);
  tcase_add_test(tc_guid, test_seq_gen_guid_between_siblings);
  tcase_add_test(tc_guid, test_seq_gen_guid_between_siblings_no_space);
  tcase_add_test(tc_guid, test_seq_gen_guid_between_parent_child);
  tcase_add_test(tc_guid, test_seq_gen_guid_between_uncle_nephew);
  tcase_add_test(tc_guid, test_seq_gen_guid_between_nephew_uncle);
  tcase_add_test(tc_guid, test_seq_gen_guid_between_same_key_different_uid);
  suite_add_tcase(s, tc_guid);

  tcase_add_test(tc_find, test_seq_iindex_of_even);
  tcase_add_test(tc_find, test_seq_iindex_of_odd);
  tcase_add_test(tc_find, test_seq_iindex_of_even_non_existent);
  tcase_add_test(tc_find, test_seq_iindex_of_odd_non_existent);
  // suite_add_tcase(s, tc_find);

  tcase_add_test(tc_insert_delete, test_seq_insert);
  tcase_add_test(tc_insert_delete, test_seq_delete);
  tcase_add_test(tc_insert_delete, test_seq_remote_insert);
  tcase_add_test(tc_insert_delete, test_seq_remote_delete);
  // suite_add_tcase(s, tc_insert_delete);

  return s;
}

int main(void) {
  int no_failed = 0;
  Suite *s;
  SRunner *runner;

  s = sequence_suite();
  runner = srunner_create(s);

  srunner_run_all(runner, CK_NORMAL);
  no_failed = srunner_ntests_failed(runner);
  srunner_free(runner);
  return (no_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

