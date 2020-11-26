#include <check.h>
#include <stdlib.h>

#include <arraylist.h>
#include <element.h>
#include <guid.h>
#include <sequence.h>

START_TEST(test_seq_gen_char) {
  Sequence* s1 = seq_new();
  char* data = "oof this is going to be a doozy...";
  int n = strlen(data);
  Element inserted[40];
  // Stage one arrange
  for (int i = 0; i < n; i++) {
    seq_insert_value_save(s1, data[i], i, inserted + i);
  }
  // Act
  char result1[40] = { 0 };
  seq_gen_chars(s1, result1);
  // Assert
  ck_assert_str_eq(result1, data);

  // Stage two arrange
  Sequence* s2 = seq_new();
  // Act
  for (int i = 0; i < n; i++) {
    seq_remote_insert(s2, &inserted[i]);
  }
  char result2[40] = { 0 };
  seq_gen_chars(s2, result2);
  // Assert
  ck_assert_str_eq(result2, data);

  seq_free(&s1);
  seq_free(&s2);
} END_TEST

Suite* sequence_display_suite(void) {
  Suite *s;
  TCase *tc_core;

  s = suite_create("sequence_display_suite");
  tc_core = tcase_create("core");

  tcase_add_test(tc_core, test_seq_gen_char);
  suite_add_tcase(s, tc_core);

  return s;
}

