#include <check.h>
#include <stdlib.h>

#include "check_arraylist_init.c"
#include "check_arraylist_insert_delete.c"
#include "check_guid_compare.c"
#include "check_guid_gen.c"
#include "check_sequence_display.c"
#include "check_sequence_guid_gen.c"
#include "check_sequence_indexing.c"
#include "check_sequence_insert_delete.c"

int main(void) {
  int no_failed;
  SRunner *runner;

  runner = srunner_create(arraylist_init_suite());
  srunner_add_suite(runner, arraylist_insert_delete_suite());
  srunner_add_suite(runner, guid_compare_suite());
  srunner_add_suite(runner, guid_gen_suite());
  srunner_add_suite(runner, sequence_display_suite());
  srunner_add_suite(runner, sequence_guid_suite());
  srunner_add_suite(runner, sequence_indexing_suite());
  srunner_add_suite(runner, sequence_insert_delete_suite());

  srunner_run_all(runner, CK_NORMAL);

  no_failed = srunner_ntests_failed(runner);
  srunner_free(runner);
  return (no_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

