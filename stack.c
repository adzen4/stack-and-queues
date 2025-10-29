/*
 * stack.c -- the implementation of stack.h
 *
 * Artsiom Dzenisiuk 16141253
 * Universiteit van Amsterdam
 */

#include <stdio.h>
#include <stdlib.h>

#include "stack.h"

/**
 * struct stack -- the structure where the stack is stored.
 * @length: the number of items currently on the stack
 * @capacity: the maximum number of items that can be stored
 * @push: the number of times the stack has been pushed to
 * @pop: the number of times the stack has been popped
 * @max: the maximum @length that has been reached
 * @data: a pointer to the items on the stack
 *
 * This is a straightforward implementation of a stack. Since the stack
 * may be resized, we cannot store the items inside the structure itself,
 * and instead we have to store the items in another region in memory.
 */
struct stack {
    size_t length;
    size_t capacity;
    size_t push;
    size_t pop;
    size_t max;
    int *data;
};

struct stack *stack_init(size_t capacity) {
    struct stack *s = malloc(sizeof(struct stack));
    if (s == NULL) {
        return NULL;
    }

    s->data = malloc(capacity * sizeof(int));
    if (s->data == NULL) {
        free(s);
        return NULL;
    }

    s->length = 0;
    s->capacity = capacity;
    s->push = 0;
    s->pop = 0;
    s->max = 0;

    return s;
}

void stack_cleanup(struct stack *s) {
    if (s == NULL) {
        return;
    }

    free(s->data);
    free(s);
}

void stack_stats(const struct stack *s) {
    if (s == NULL) {
        return;
    }

    fprintf(stderr, "stats %zu %zu %zu\n", s->push, s->pop, s->max);
}

int stack_push(struct stack *s, int c) {
    if (s == NULL) {
        return 1;
    }

    if (s->length >= s->capacity) {
        size_t new_capacity = s->capacity * 2 + 1;
        int *new = realloc(s->data, new_capacity * sizeof(int));
        if (new == NULL) {
            return 1;
        }

        s->capacity = new_capacity;
        s->data = new;
    }

    s->data[s->length++] = c;
    s->push++;

    if (s->length > s->max) {
        s->max = s->length;
    }

    return 0;
}

int stack_pop(struct stack *s) {
    if (s == NULL) {
        return -1;
    }

    if (s->length == 0) {
        return -1;
    }

    int value = s->data[--s->length];
    s->pop++;

    return value;
}

int stack_peek(const struct stack *s) {
    if (s == NULL) {
        return -1;
    }

    return s->data[s->length - 1];
}

int stack_empty(const struct stack *s) {
    if (s == NULL) {
        return -1;
    }

    return s->length == 0;
}

size_t stack_size(const struct stack *s) {
    if (s == NULL) {
        return 1;
    }

    return s->length;
}
