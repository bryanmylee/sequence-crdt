#include <check.h>
#include <stdlib.h>

#include <sequence.h>

START_TEST(test_seq_iindex_of_even) {
  Sequence *s = seq_new();
  // remove header and trailer elements for the purpose of this test.
  al_init(&s->elements, sizeof(Element));
  // insert 8 Element pointers.
  for (int i = 0; i < 8; i++) {
    // give all elements a simple incremental guid.
    // at depth 3, all 8 elements can fit under one node.
    Element e = {
      .id = (Guid) {
        .depth = 3,
        .keys = keys_from_tokens(3, 0, 0, i),
      },
    };
    e.data.value = i;
    al_add(&s->elements, &e);
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
  Sequence *s = seq_new();
  // remove header and trailer elements for the purpose of this test.
  al_init(&s->elements, sizeof(Element));
  // insert 7 Element pointers.
  for (int i = 0; i < 7; i++) {
    Element e = {
      .id = (Guid) {
        .depth = 3,
        .keys = keys_from_tokens(3, 0, 0, i),
      },
    };
    e.data.value = i;
    al_add(&s->elements, &e);
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
  Sequence *s = seq_new();
  // remove header and trailer elements for the purpose of this test.
  al_init(&s->elements, sizeof(Element));
  // insert 8 Element pointers with even key tokens.
  for (int i = 0; i < 8; i++) {
    // give all elements a simple incremental guid.
    // at depth 4, there are 16 possible locations.
    Element e = {
      .id = (Guid) {
        .depth = 4,
        .keys = keys_from_tokens(4, 0, 0, 0, i * 2),
      },
    };
    e.data.value = i;
    al_add(&s->elements, &e);
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
  Sequence *s = seq_new();
  // remove header and trailer elements for the purpose of this test.
  al_init(&s->elements, sizeof(Element));
  // insert 7 Element pointers with even key tokens.
  for (int i = 0; i < 7; i++) {
    // give all elements a simple incremental guid.
    // at depth 4, there are 16 possible locations.
    Element e = {
      .id = (Guid) {
        .depth = 4,
        .keys = keys_from_tokens(4, 0, 0, 0, i * 2),
      },
    };
    e.data.value = i;
    al_add(&s->elements, &e);
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

Suite *sequence_indexing_suite(void) {
  Suite *s;
  TCase *tc_core;

  s = suite_create("check_sequence_indexing");
  tc_core = tcase_create("core");

  tcase_add_test(tc_core, test_seq_iindex_of_even);
  tcase_add_test(tc_core, test_seq_iindex_of_odd);
  tcase_add_test(tc_core, test_seq_iindex_of_even_non_existent);
  tcase_add_test(tc_core, test_seq_iindex_of_odd_non_existent);
  suite_add_tcase(s, tc_core);

  return s;
}

