#include <check.h>
#include <stdio.h>
#include <stdlib.h>

#include "queue.h"

/* For older versions of the check library */
#ifndef ck_assert_ptr_nonnull
#define ck_assert_ptr_nonnull(X) _ck_assert_ptr(X, !=, NULL)
#endif
#ifndef ck_assert_ptr_null
#define ck_assert_ptr_null(X) _ck_assert_ptr(X, ==, NULL)
#endif


START_TEST(test_queue_init_cleanup) {
    struct queue *q = queue_init(10);
    ck_assert_ptr_nonnull(q);
    queue_cleanup(q);
}
END_TEST

START_TEST(test_queue_push_return) {
    struct queue *q = queue_init(10);
    ck_assert_int_eq(queue_push(q, 'x'), 0);
    queue_cleanup(q);
}
END_TEST

START_TEST(test_queue_pop_simple) {
    struct queue *q = queue_init(10);
    ck_assert_int_eq(queue_push(q, 'x'), 0);
    ck_assert_int_eq(queue_pop(q), 'x');

    ck_assert_int_eq(queue_push(q, 'y'), 0);
    ck_assert_int_eq(queue_pop(q), 'y');
    queue_cleanup(q);
}
END_TEST

START_TEST(test_queue_order) {
    struct queue *q = queue_init(10);
    ck_assert_int_eq(queue_push(q, 'x'), 0);
    ck_assert_int_eq(queue_push(q, 'y'), 0);
    ck_assert_int_eq(queue_push(q, 'z'), 0);

    ck_assert_int_eq(queue_pop(q), 'x');
    ck_assert_int_eq(queue_pop(q), 'y');
    ck_assert_int_eq(queue_pop(q), 'z');
    queue_cleanup(q);
}
END_TEST

START_TEST(test_queue_push_pop) {
    struct queue *q = queue_init(10);
    ck_assert_int_eq(queue_push(q, 'x'), 0);
    ck_assert_int_eq(queue_push(q, 'y'), 0);
    ck_assert_int_eq(queue_push(q, 'z'), 0);

    ck_assert_int_eq(queue_pop(q), 'x');
    ck_assert_int_eq(queue_pop(q), 'y');
    ck_assert_int_eq(queue_push(q, 'a'), 0);
    ck_assert_int_eq(queue_push(q, 'b'), 0);
    ck_assert_int_eq(queue_pop(q), 'z');
    ck_assert_int_eq(queue_push(q, 'c'), 0);

    ck_assert_int_eq(queue_pop(q), 'a');
    ck_assert_int_eq(queue_pop(q), 'b');
    ck_assert_int_eq(queue_pop(q), 'c');
    queue_cleanup(q);
}
END_TEST

START_TEST(test_queue_peek) {
    struct queue *q = queue_init(10);
    ck_assert_int_eq(queue_push(q, 'x'), 0);
    ck_assert_int_eq(queue_push(q, 'y'), 0);
    ck_assert_int_eq(queue_push(q, 'z'), 0);

    ck_assert_int_eq(queue_peek(q), 'x');
    ck_assert_int_eq(queue_peek(q), 'x');
    ck_assert_int_eq(queue_pop(q), 'x');

    ck_assert_int_eq(queue_peek(q), 'y');
    ck_assert_int_eq(queue_pop(q), 'y');

    ck_assert_int_eq(queue_peek(q), 'z');
    queue_cleanup(q);
}
END_TEST

START_TEST(test_queue_empty) {
    struct queue *q = queue_init(10);
    ck_assert_int_eq(queue_empty(q), 1);

    ck_assert_int_eq(queue_push(q, 'x'), 0);
    ck_assert_int_eq(queue_empty(q), 0);

    ck_assert_int_eq(queue_pop(q), 'x');
    ck_assert_int_eq(queue_empty(q), 1);
    queue_cleanup(q);
}
END_TEST

/* If you implement queue resizing (bonus) uncomment this test. */
/*
START_TEST(test_queue_realloc) {
    struct queue *q = queue_init(5);
    for (int i = 0; i < 15; i++) {
        ck_assert_int_eq(queue_push(q, i), 0);
    }

    for (int i = 0; i < 15; i++) {
        ck_assert_int_eq(queue_pop(q), i);
    }

    for (int i = 0; i < 30; i++) {
        ck_assert_int_eq(queue_push(q, i), 0);
    }

    for (int i = 0; i < 30; i++) {
        ck_assert_int_eq(queue_pop(q), i);
    }
    queue_cleanup(q);
}
END_TEST

START_TEST(test_queue_realloc_after_pop) {
    struct queue *q = queue_init(4);
    for (int i = 0; i < 4; i++) {
        ck_assert_int_eq(queue_push(q, i), 0);
    }

    for (int i = 0; i < 2; i++) {
        ck_assert_int_eq(queue_pop(q), i);
    }

    for (int i = 0; i < 20; i++) {
        ck_assert_int_eq(queue_push(q, i), 0);
    }

    for (int i = 2; i < 4; i++) {
        ck_assert_int_eq(queue_pop(q), i);
    }

    for (int i = 0; i < 20; i++) {
        ck_assert_int_eq(queue_pop(q), i);
    }
    queue_cleanup(q);
}
END_TEST

START_TEST(test_queue_realloc_zero_size_queue) {
    struct queue *q = queue_init(0);
    for (int i = 0; i < 100000; i++) {
        ck_assert_int_eq(queue_push(q, i), 0);
    }

    for (int i = 0; i < 100000; i++) {
        ck_assert_int_eq(queue_pop(q), i);
    }

    queue_cleanup(q);
}
END_TEST
*/

START_TEST(test_queue_overflow) {
    struct queue *s = queue_init(5);
    int realloc_p = 0;
    for (int i = 0; i < 15; i++) {
        int r = queue_push(s, i);
        if (i <= 4) { /* Should succeed. */
            ck_assert(r == 0);
        } else { /* Can succeed if queue grows. (bonus) */
            if (r == 0) {
                realloc_p = 1;
            } else if (r != 1) {
                ck_assert(0);
            }
        }
    }

    if (realloc_p) {
        /* Queue does grows if capacity is reached. (bonus) */
        for (int i = 0; i < 15; i++) {
            ck_assert_int_eq(queue_pop(s), i);
        }
    } else {
        /* Queue does not grow. */
        for (int i = 0; i < 5; i++) {
            ck_assert_int_eq(queue_pop(s), i);
        }
    }

    queue_cleanup(s);
}
END_TEST

START_TEST(test_queue_underflow) {
    struct queue *q = queue_init(10);
    for (int i = 0; i < 10; i++) {
        ck_assert_int_eq(queue_pop(q), -1);
    }

    for (int i = 0; i < 10; i++) {
        ck_assert_int_eq(queue_push(q, i), 0);
    }

    for (int i = 0; i < 10; i++) {
        ck_assert_int_eq(queue_pop(q), i);
    }

    for (int i = 0; i < 10; i++) {
        ck_assert_int_eq(queue_pop(q), -1);
    }
    queue_cleanup(q);
}
END_TEST

START_TEST(test_queue_multiple_queues) {
    struct queue *q1 = queue_init(10);
    struct queue *q2 = queue_init(10);
    ck_assert_int_eq(queue_push(q1, 'x'), 0);
    ck_assert_int_eq(queue_push(q1, 'y'), 0);
    ck_assert_int_eq(queue_push(q1, 'z'), 0);

    ck_assert_int_eq(queue_push(q2, 'a'), 0);
    ck_assert_int_eq(queue_push(q2, 'b'), 0);
    ck_assert_int_eq(queue_push(q2, 'c'), 0);

    ck_assert_int_eq(queue_pop(q1), 'x');
    ck_assert_int_eq(queue_pop(q1), 'y');
    ck_assert_int_eq(queue_pop(q1), 'z');

    ck_assert_int_eq(queue_pop(q2), 'a');
    ck_assert_int_eq(queue_pop(q2), 'b');
    ck_assert_int_eq(queue_pop(q2), 'c');

    ck_assert_int_eq(queue_empty(q1), 1);
    ck_assert_int_eq(queue_empty(q2), 1);

    queue_cleanup(q1);
    queue_cleanup(q2);
}
END_TEST


START_TEST(test_queue_null_ptr) {
    ck_assert_int_eq(queue_push(NULL, 'x'), 1);
    ck_assert_int_eq(queue_pop(NULL), -1);
    ck_assert_int_eq(queue_peek(NULL), -1);
    ck_assert_int_eq(queue_empty(NULL), -1);
}
END_TEST

Suite *queue_suite(void) {
    Suite *s;
    TCase *tc_core;
    TCase *tc_limits;
    s = suite_create("queue");

    tc_core = tcase_create("Core");
    tcase_add_test(tc_core, test_queue_init_cleanup);
    tcase_add_test(tc_core, test_queue_push_return);
    tcase_add_test(tc_core, test_queue_pop_simple);
    tcase_add_test(tc_core, test_queue_order);
    tcase_add_test(tc_core, test_queue_push_pop);
    tcase_add_test(tc_core, test_queue_peek);
    tcase_add_test(tc_core, test_queue_empty);

    tc_limits = tcase_create("Limits");
    tcase_add_test(tc_limits, test_queue_overflow);
    tcase_add_test(tc_limits, test_queue_underflow);
    tcase_add_test(tc_limits, test_queue_multiple_queues);
    tcase_add_test(tc_limits, test_queue_null_ptr);

    suite_add_tcase(s, tc_core);
    suite_add_tcase(s, tc_limits);

    /* If you implement queue resizing (bonus) uncomment the test
     * 'test_queue_realloc*' tests above and the lines below. */
    // TCase *tc_bonus;
    // tc_bonus = tcase_create("Bonus");
    // tcase_add_test(tc_bonus, test_queue_realloc);
    // tcase_add_test(tc_bonus, test_queue_realloc_after_pop);
    // tcase_add_test(tc_bonus, test_queue_realloc_zero_size_queue);
    // suite_add_tcase(s, tc_bonus);

    return s;
}

int main(void) {
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = queue_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_VERBOSE);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
