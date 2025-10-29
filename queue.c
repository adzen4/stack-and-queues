#include <stdio.h>
#include <stdlib.h>

#include "queue.h"

struct queue {
    /* ... SOME CODE MISSING HERE ... */
};

struct queue *queue_init(size_t capacity) {
    /* ... SOME CODE MISSING HERE ... */
}

void queue_cleanup(struct queue *q) {
    /* ... SOME CODE MISSING HERE ... */
}

void queue_stats(const struct queue *q) {
    /* ... SOME CODE MISSING HERE ... */
}

int queue_push(struct queue *q, int e) {
    /* ... SOME CODE MISSING HERE ... */
}

int queue_pop(struct queue *q) {
    /* ... SOME CODE MISSING HERE ... */
}

int queue_peek(const struct queue *q) {
    /* ... SOME CODE MISSING HERE ... */
}

int queue_empty(const struct queue *q) {
    /* ... SOME CODE MISSING HERE ... */
}

size_t queue_size(const struct queue *q) {
    /* ... SOME CODE MISSING HERE ... */
}
