#include <check.h>
#include <stdio.h>
#include <stdlib.h>

#include "queue.h"
#include "stack.h"

/* For older versions of the check library */
#ifndef ck_assert_ptr_nonnull
#define ck_assert_ptr_nonnull(X) _ck_assert_ptr(X, !=, NULL)
#endif
#ifndef ck_assert_ptr_null
#define ck_assert_ptr_null(X) _ck_assert_ptr(X, ==, NULL)
#endif

START_TEST(test_stack_cleanup) {
    stack_cleanup(NULL);
}
END_TEST

START_TEST(test_stack_stats) {
    stack_stats(NULL);
}
END_TEST

START_TEST(test_stack_push) {
    ck_assert_int_eq(stack_push(NULL, 0), 1);
}
END_TEST

START_TEST(test_stack_pop) {
    ck_assert_int_eq(stack_pop(NULL), -1);
}
END_TEST

START_TEST(test_stack_peek) {
    ck_assert_int_eq(stack_peek(NULL), -1);
}
END_TEST

START_TEST(test_stack_empty) {
    ck_assert_int_eq(stack_empty(NULL), -1);
}
END_TEST

START_TEST(test_stack_size) {
    stack_size(NULL);
}
END_TEST

START_TEST(test_queue_cleanup) {
    queue_cleanup(NULL);
}
END_TEST

START_TEST(test_queue_stats) {
    queue_stats(NULL);
}
END_TEST

START_TEST(test_queue_push) {
    ck_assert_int_eq(queue_push(NULL, 2), 1);
}
END_TEST

START_TEST(test_queue_pop) {
    ck_assert_int_eq(queue_pop(NULL), -1);
}
END_TEST

START_TEST(test_queue_peek) {
    ck_assert_int_eq(queue_peek(NULL), -1);
}
END_TEST

START_TEST(test_queue_empty) {
    ck_assert_int_eq(queue_empty(NULL), -1);
}
END_TEST

START_TEST(test_queue_size) {
    queue_size(NULL);
}
END_TEST

Suite *null_suite(void) {
    Suite *s;
    TCase *tc_stack;
    TCase *tc_queue;
    s = suite_create("null");

    tc_stack = tcase_create("Stack");
    tcase_add_test(tc_stack, test_stack_cleanup);
    tcase_add_test(tc_stack, test_stack_stats);
    tcase_add_test(tc_stack, test_stack_push);
    tcase_add_test(tc_stack, test_stack_pop);
    tcase_add_test(tc_stack, test_stack_peek);
    tcase_add_test(tc_stack, test_stack_empty);
    tcase_add_test(tc_stack, test_stack_size);

    tc_queue = tcase_create("Queue");
    tcase_add_test(tc_queue, test_queue_cleanup);
    tcase_add_test(tc_queue, test_queue_stats);
    tcase_add_test(tc_queue, test_queue_push);
    tcase_add_test(tc_queue, test_queue_pop);
    tcase_add_test(tc_queue, test_queue_peek);
    tcase_add_test(tc_queue, test_queue_empty);
    tcase_add_test(tc_queue, test_queue_size);


    suite_add_tcase(s, tc_stack);
    suite_add_tcase(s, tc_queue);
    return s;
}

int main(void) {
    int number_failed, number_total;
    Suite *s;
    SRunner *sr;

    s = null_suite();
    sr = srunner_create(s);
    srunner_set_xml(sr, getenv("CG_JUNIT_XML_LOCATION"));

    srunner_run_all(sr, CK_VERBOSE);
    number_failed = srunner_ntests_failed(sr);
    number_total = srunner_ntests_run(sr);
    printf("score: %f\n", 1.0 - ((float) number_failed / (float)number_total));
    srunner_free(sr);
    return 0;
}
