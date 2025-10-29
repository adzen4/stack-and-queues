#include <check.h>
#include <stdio.h>
#include <stdlib.h>

#include "stack.h"

/* For older versions of the check library */
#ifndef ck_assert_ptr_nonnull
#define ck_assert_ptr_nonnull(X) _ck_assert_ptr(X, !=, NULL)
#endif
#ifndef ck_assert_ptr_null
#define ck_assert_ptr_null(X) _ck_assert_ptr(X, ==, NULL)
#endif


START_TEST(test_stack_init_cleanup) {
    struct stack *s = stack_init(10);
    ck_assert_ptr_nonnull(s);
    stack_cleanup(s);
}
END_TEST

START_TEST(test_stack_push_return) {
    struct stack *s = stack_init(10);
    ck_assert_int_eq(stack_push(s, 'x'), 0);
    stack_cleanup(s);
}
END_TEST

START_TEST(test_stack_pop_simple) {
    struct stack *s = stack_init(10);
    ck_assert_int_eq(stack_push(s, 'x'), 0);
    ck_assert_int_eq(stack_pop(s), 'x');

    ck_assert_int_eq(stack_push(s, 'y'), 0);
    ck_assert_int_eq(stack_pop(s), 'y');
    stack_cleanup(s);
}
END_TEST

START_TEST(test_stack_order) {
    struct stack *s = stack_init(10);
    ck_assert_int_eq(stack_push(s, 'x'), 0);
    ck_assert_int_eq(stack_push(s, 'y'), 0);
    ck_assert_int_eq(stack_push(s, 'z'), 0);

    ck_assert_int_eq(stack_pop(s), 'z');
    ck_assert_int_eq(stack_pop(s), 'y');
    ck_assert_int_eq(stack_pop(s), 'x');
    stack_cleanup(s);
}
END_TEST

START_TEST(test_stack_push_pop) {
    struct stack *s = stack_init(10);
    ck_assert_int_eq(stack_push(s, 'x'), 0);
    ck_assert_int_eq(stack_push(s, 'y'), 0);
    ck_assert_int_eq(stack_push(s, 'z'), 0);

    ck_assert_int_eq(stack_pop(s), 'z');
    ck_assert_int_eq(stack_pop(s), 'y');
    ck_assert_int_eq(stack_push(s, 'a'), 0);
    ck_assert_int_eq(stack_push(s, 'b'), 0);
    ck_assert_int_eq(stack_pop(s), 'b');
    ck_assert_int_eq(stack_push(s, 'c'), 0);

    ck_assert_int_eq(stack_pop(s), 'c');
    ck_assert_int_eq(stack_pop(s), 'a');
    ck_assert_int_eq(stack_pop(s), 'x');
    stack_cleanup(s);
}
END_TEST

START_TEST(test_stack_peek) {
    struct stack *s = stack_init(10);
    ck_assert_int_eq(stack_push(s, 'x'), 0);
    ck_assert_int_eq(stack_push(s, 'y'), 0);
    ck_assert_int_eq(stack_push(s, 'z'), 0);

    ck_assert_int_eq(stack_peek(s), 'z');
    ck_assert_int_eq(stack_peek(s), 'z');
    ck_assert_int_eq(stack_pop(s), 'z');

    ck_assert_int_eq(stack_peek(s), 'y');
    ck_assert_int_eq(stack_pop(s), 'y');

    ck_assert_int_eq(stack_peek(s), 'x');
    stack_cleanup(s);
}
END_TEST

START_TEST(test_stack_empty) {
    struct stack *s = stack_init(10);
    ck_assert_int_eq(stack_empty(s), 1);

    ck_assert_int_eq(stack_push(s, 'x'), 0);
    ck_assert_int_eq(stack_empty(s), 0);

    ck_assert_int_eq(stack_pop(s), 'x');
    ck_assert_int_eq(stack_empty(s), 1);
    stack_cleanup(s);
}
END_TEST

START_TEST(test_stack_overflow) {
    struct stack *s = stack_init(5);
    int realloc_p = 0;
    for (int i = 0; i < 15; i++) {
        int r = stack_push(s, i);
        if (i <= 4) { /* Should succeed. */
            ck_assert(r == 0);
        } else { /* Can succeed if stack grows. (bonus) */
            if (r == 0) {
                realloc_p = 1;
            } else if (r != 1) {
                ck_assert(0);
            }
        }
    }

    if (realloc_p) {
        /* Stack does grows if capacity is reached. (bonus) */
        for (int i = 14; i >= 0; i--) {
            ck_assert_int_eq(stack_pop(s), i);
        }
    } else {
        /* Stack does not grow. */
        for (int i = 4; i >= 0; i--) {
            ck_assert_int_eq(stack_pop(s), i);
        }
    }

    stack_cleanup(s);
}
END_TEST


/* If you implement stack resizing (bonus) uncomment this test. */
/*
START_TEST(test_stack_realloc) {
    struct stack *s = stack_init(5);
    for (int i = 0; i < 15; i++) {
        ck_assert_int_eq(stack_push(s, i), 0);
    }

    for (int i = 14; i >= 0; i--) {
        ck_assert_int_eq(stack_pop(s), i);
    }

    for (int i = 0; i < 100; i++) {
        ck_assert_int_eq(stack_push(s, i), 0);
    }

    for (int i = 99; i >= 0; i--) {
        ck_assert_int_eq(stack_pop(s), i);
    }
    stack_cleanup(s);
}
END_TEST
*/

START_TEST(test_stack_underflow) {
    struct stack *s = stack_init(10);
    for (int i = 0; i < 10; i++) {
        ck_assert_int_eq(stack_pop(s), -1);
    }

    for (int i = 0; i < 10; i++) {
        ck_assert_int_eq(stack_push(s, i), 0);
    }

    for (int i = 9; i >= 0; i--) {
        ck_assert_int_eq(stack_pop(s), i);
    }

    for (int i = 0; i < 10; i++) {
        ck_assert_int_eq(stack_pop(s), -1);
    }
    stack_cleanup(s);
}
END_TEST

START_TEST(test_stack_multiple_stacks) {
    struct stack *s1 = stack_init(10);
    struct stack *s2 = stack_init(10);
    ck_assert_int_eq(stack_push(s1, 'x'), 0);
    ck_assert_int_eq(stack_push(s1, 'y'), 0);
    ck_assert_int_eq(stack_push(s1, 'z'), 0);

    ck_assert_int_eq(stack_push(s2, 'a'), 0);
    ck_assert_int_eq(stack_push(s2, 'b'), 0);
    ck_assert_int_eq(stack_push(s2, 'c'), 0);

    ck_assert_int_eq(stack_pop(s1), 'z');
    ck_assert_int_eq(stack_pop(s1), 'y');
    ck_assert_int_eq(stack_pop(s1), 'x');

    ck_assert_int_eq(stack_pop(s2), 'c');
    ck_assert_int_eq(stack_pop(s2), 'b');
    ck_assert_int_eq(stack_pop(s2), 'a');

    ck_assert_int_eq(stack_empty(s1), 1);
    ck_assert_int_eq(stack_empty(s2), 1);

    stack_cleanup(s1);
    stack_cleanup(s2);
}
END_TEST


START_TEST(test_stack_null_ptr) {
    ck_assert_int_eq(stack_push(NULL, 'x'), 1);
    ck_assert_int_eq(stack_pop(NULL), -1);
    ck_assert_int_eq(stack_peek(NULL), -1);
    ck_assert_int_eq(stack_empty(NULL), -1);
}
END_TEST

Suite *stack_suite(void) {
    Suite *s;
    TCase *tc_core;
    TCase *tc_limits;
    s = suite_create("Stack");

    tc_core = tcase_create("Core");
    tcase_add_test(tc_core, test_stack_init_cleanup);
    tcase_add_test(tc_core, test_stack_push_return);
    tcase_add_test(tc_core, test_stack_pop_simple);
    tcase_add_test(tc_core, test_stack_order);
    tcase_add_test(tc_core, test_stack_push_pop);
    tcase_add_test(tc_core, test_stack_peek);
    tcase_add_test(tc_core, test_stack_empty);

    tc_limits = tcase_create("Limits");
    tcase_add_test(tc_limits, test_stack_overflow);
    tcase_add_test(tc_limits, test_stack_underflow);
    tcase_add_test(tc_limits, test_stack_multiple_stacks);
    tcase_add_test(tc_limits, test_stack_null_ptr);

    suite_add_tcase(s, tc_core);
    suite_add_tcase(s, tc_limits);

    /* If you implement stack resizing (bonus) uncomment the test
     * 'test_stack_realloc' above and the lines below. */
    // TCase *tc_bonus;
    // tc_bonus = tcase_create("Bonus");
    // tcase_add_test(tc_bonus, test_stack_realloc);
    // suite_add_tcase(s, tc_bonus);

    return s;
}

int main(void) {
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = stack_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_VERBOSE);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
