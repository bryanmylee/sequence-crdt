#include <check.h>
#include <model/arraylist.h>
#include <model/element.h>
#include <model/guid.h>
#include <model/sequence.h>
#include <stdlib.h>

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
}

Suite* sequence_display_suite(void) {
  Suite *s;
  TCase *tc_display;

  s = suite_create("sequence_display_suite");
  tc_display = tcase_create("display");

  tcase_add_test(tc_display, test_seq_gen_char);
  suite_add_tcase(s, tc_display);

  return s;
}

int main(void) {
  int no_failed = 0;
  Suite *s;
  SRunner *runner;

  s = sequence_display_suite();
  runner = srunner_create(s);

  srunner_run_all(runner, CK_NORMAL);
  no_failed = srunner_ntests_failed(runner);
  srunner_free(runner);
  return (no_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

