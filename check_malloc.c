#define _GNU_SOURCE
#include <check.h>
#include <dlfcn.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "stack.h"
#include "queue.h"

// For older versions of the check library
#ifndef ck_assert_ptr_nonnull
#define ck_assert_ptr_nonnull(X) _ck_assert_ptr(X, !=, NULL)
#endif
#ifndef ck_assert_ptr_null
#define ck_assert_ptr_null(X) _ck_assert_ptr(X, ==, NULL)
#endif

#define ARRAY_SIZE (4096)

#define RESET_VAL(val) (val = 2)

volatile char _exit_worksp = 1;
volatile char _malloc_failp = 0;
volatile char _malloc_failedp = 0;
volatile size_t malloc_non_fail_amount = 0;
volatile size_t _malloc_fail_amount = 0;
volatile size_t free_count = 0;
volatile const char exit_code = 2;

static inline void enable_malloc_failing() {
    _malloc_fail_amount = 0;
    malloc_non_fail_amount = 0;
    _exit_worksp = 0;
    _malloc_failedp = 0;
    _malloc_failp = 1;
}

static char malloc_failedp() {
    char res = _malloc_failedp;
    _malloc_failedp = 0;
    free_count = 0;
    if (res) {
        _malloc_fail_amount++;
        malloc_non_fail_amount = 0;
    }
    return res;
}

static void restore_malloc() {
    free_count = 0;
    _exit_worksp = 1;
    _malloc_failp = 0;
}

static void *(*real_malloc)(size_t) = NULL;
static void (*real_free)(void *) = NULL;

static void mtrace_init(void) {
    real_malloc = dlsym(RTLD_NEXT, "malloc");
    real_free = dlsym(RTLD_NEXT, "free");
    if (NULL == real_malloc || NULL == real_free) {
        fprintf(stderr, "Error in `dlsym`: %s\n", dlerror());
    }
    printf("Tracing\n");
}

void *malloc(size_t sz) {
    if (NULL == real_malloc) {
        mtrace_init();
    }

    if (_malloc_failp && (++malloc_non_fail_amount > _malloc_fail_amount)) {
        _malloc_failedp = 1;
        return NULL;
    }

    return real_malloc(sz);
}

// Disable perror because it confuses our malloc checking
void perror(const char *s) {
    return;
}

void free(void *pt) {
    if (_malloc_failp) {
        ++free_count;
    }
    real_free(pt);
}

// our temporary calloc used until we get the address of libc provided
// one in our interposed calloc
static void *temporary_calloc(size_t x, size_t y) {
    (void)x;
    (void)y;
    return NULL;
}

void *calloc(size_t nelements, size_t elementSize) {
    static void *(*calloc_func)(size_t, size_t) = 0;

    if (!calloc_func) {
        // before trying to get the libc provided
        // calloc, set the calloc function to the temporary
        // one returning NULL.
        calloc_func = temporary_calloc;

        // dlsym will call again this calloc in which we are,
        // but seems like it handles just fine if we are
        // returning NULL
        calloc_func = (void *(*)(size_t, size_t))dlsym(RTLD_NEXT, "calloc");
    }
    if (_malloc_failp && (++malloc_non_fail_amount > _malloc_fail_amount)) {
        _malloc_failedp = 1;
        return NULL;
    }

    return calloc_func(nelements, elementSize);
}

void exit(int status) {
    void (*libc_exit)(int status) = dlsym(RTLD_NEXT, "exit");
    libc_exit(_exit_worksp ? status : (exit_code + 1));
    while (1)
        ;
}

void _exit(int status) {
    exit(status);
}

#define MALLOC_LOOP(res, init, err)                                                       \
    do {                                                                                  \
        res = init;                                                                       \
        size_t old_non_fail_amount = malloc_non_fail_amount, old_free_count = free_count; \
        if (err == res) {                                                                 \
            if (old_non_fail_amount - 1 != old_free_count) {                              \
                restore_malloc();                                                         \
                exit(exit_code + 1);                                                      \
            }                                                                             \
        } else {                                                                          \
            if (malloc_failedp()) {                                                       \
                restore_malloc();                                                         \
                exit(exit_code + 1);                                                      \
            }                                                                             \
        }                                                                                 \
    } while (malloc_failedp())

START_TEST(test_stack_init_no_malloc) {
    struct stack *s = NULL;

    enable_malloc_failing();

    MALLOC_LOOP(s, stack_init(5), NULL);

    restore_malloc();

    if (stack_push(s, 10))
        ck_assert(0);

    if (stack_peek(s) != 10)
        ck_assert(0);

    if (stack_pop(s) != 10)
        ck_assert(0);

    stack_cleanup(s);
    exit(exit_code);
}
END_TEST

START_TEST(test_queue_init_no_malloc) {
    struct queue *q = NULL;

    enable_malloc_failing();

    MALLOC_LOOP(q, queue_init(5), NULL);

    restore_malloc();

    if (queue_push(q, 10))
        ck_assert(0);

    if (queue_peek(q) != 10)
        ck_assert(0);

    if (queue_pop(q) != 10)
        ck_assert(0);

    queue_cleanup(q);
    exit(exit_code);
}
END_TEST

Suite *heap_suite(void) {
    Suite *s;
    TCase *tc_core;

    s = suite_create("malloc checks");
    /* Core test case */
    tc_core = tcase_create("Core");

    /* Regular tests. */
    tcase_add_exit_test(tc_core, test_stack_init_no_malloc, exit_code);
    tcase_add_exit_test(tc_core, test_queue_init_no_malloc, exit_code);

    suite_add_tcase(s, tc_core);
    return s;
}

int main(void) {
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = heap_suite();
    sr = srunner_create(s);

    /* srunner_run_all(sr, CK_NORMAL); */
    srunner_run_all(sr, CK_VERBOSE);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    if (number_failed) {
        fprintf(stderr, "You forgot to check a function for error codes!\n");
    }
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
