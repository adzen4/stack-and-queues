/*
 * queue.c -- the implementation of queue.h
 *
 * Artsiom Dzenisiuk 16141253
 * Universiteit van Amsterdam
 */

#include <stdio.h>
#include <stdlib.h>

#include "queue.h"

/**
 * struct queue -- the struct where the queue is stored
 * @length: the number of items currently in the queue
 * @capacity: the maximum number of items that can be stored
 * @head: the index of the head
 * @tail: the index of the tail
 * @push: the number of times the queue has been pushed to
 * @pop: the number of times the queue has been popped
 * @max: the maximum @length that has been reached
 * @data: a pointer to the items in the queue
 *
 * This is a straightforward implementation of a queue. Since the queue
 * may be resized, we cannot store the items inside the structure itself,
 * and instead we have to store the items in another region in memory.
 */
struct queue {
    size_t length;
    size_t capacity;
    size_t head;
    size_t tail;
    size_t push;
    size_t pop;
    size_t max;
    int *data;
};

struct queue *queue_init(size_t capacity) {
    struct queue *q = malloc(sizeof(struct queue));
    if (q == NULL) {
        return NULL;
    }

    q->data = malloc(capacity * sizeof(int));
    if (q->data == NULL) {
        free(q);
        return NULL;
    }

    q->length = 0;
    q->capacity = capacity;
    q->head = 0;
    q->tail = 0;
    q->push = 0;
    q->pop = 0;
    q->max = 0;

    return q;
}

void queue_cleanup(struct queue *q) {
    if (q == NULL) {
        return;
    }
    
    free(q->data);
    free(q);
}

void queue_stats(const struct queue *q) {
    if (q == NULL) {
        return;
    }
    
    fprintf(stderr, "stats %zu %zu %zu\n", q->push, q->pop, q->max);
}

int queue_push(struct queue *q, int e) {
    if (q == NULL) {
        return 1;
    }
    
    if (q->length >= q->capacity) {
        size_t new_capacity = q->capacity * 2 + 1;
        int *new = malloc(new_capacity * sizeof(int));
        if (new == NULL) {
            return 1;
        }

        /*
         * Now we need to move the data from the old array
         * into the new array. This is more complicated than regular
         * array resizing. We need to reorder the items, head and tail
         * so that they form a simple array that starts at index 0.
         */

        for (size_t i = 0; i < q->length; i++) {
            size_t old_index = i + q->tail;
            if (old_index >= q->capacity) {
                old_index -= q->capacity;
            }
            
            new[i] = q->data[old_index];
        }

        free(q->data);

        q->tail = 0;
        q->head = q->length;
        q->data = new;
        q->capacity = new_capacity;
    }

    q->data[q->head++] = e;
    
    if (q->head >= q->capacity) {
        q->head -= q->capacity;
    }

    q->length++;
    q->push++;

    if (q->length >= q->max) {
        q->max = q->length;
    }

    return 0;    
}

int queue_pop(struct queue *q) {
    if (q == NULL) {
        return -1;
    }
    
    if (q->length == 0) {
        return -1;
    }

    int value = q->data[q->tail++];

    if (q->tail >= q->capacity) {
        q->tail -= q->capacity;
    }

    q->length--;
    q->pop++;

    return value;
}

int queue_peek(const struct queue *q) {
    if (q == NULL) {
        return -1;
    }
    
    return q->data[q->tail];
}

int queue_empty(const struct queue *q) {
    if (q == NULL) {
        return -1;
    }
    
    return q->length == 0;
}

size_t queue_size(const struct queue *q) {
    if (q == NULL) {
        return 1;
    }
    
    return q->length;
}
