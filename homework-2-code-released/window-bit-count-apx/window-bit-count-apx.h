#ifndef _WINDOW_BIT_COUNT_APX_
#define _WINDOW_BIT_COUNT_APX_

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

uint64_t N_MERGES = 0; // keep track of how many bucket merges occur

/*
    TODO: You can add code here.
*/

typedef struct {
    uint32_t now;
    uint32_t k;
    uint32_t wnd_size;
    uint32_t num_buckets;
    uint32_t count;
    Bucket *buckets;
    // TODO: Fill me.
} StateApx;

typedef struct {
    bool size;
    bool increment;
    uint32_t timestamp;
} Bucket;

// k = 1/eps
// if eps = 0.01 (relative error 1%) then k = 100
// if eps = 0.001 (relative error 0.1%) the k = 1000
uint64_t wnd_bit_count_apx_new(StateApx* self, uint32_t wnd_size, uint32_t k) {
    assert(wnd_size >= 1);
    assert(k >= 1);

    // TODO: Fill me.

    uint32_t num_buckets = 1;
    uint32_t max_bits = 1;

    while (max_bits < wnd_size) {
        num_buckets++;
        max_bits *= 2;
    }

    self->now = 0;
    self->k = k;
    self->wnd_size = wnd_size;
    self->num_buckets = num_buckets;
    self->count = 0;
    self->buckets = (Bucket*) malloc(num_buckets * sizeof(Bucket));

    for (uint32_t i = 0; i < num_buckets; i++) {
        self->buckets[i].size = 0;
        self->buckets[i].increment = false;
        self->buckets[i].timestamp = 0;
    }

    // TODO:
    // The function should return the total number of bytes allocated on the heap.
    return num_buckets * sizeof(Bucket);
}

void wnd_bit_count_apx_destruct(StateApx* self) {
    // TODO: Fill me.
    free(self->buckets);
    // Make sure you free the memory allocated on the heap.
}

void wnd_bit_count_apx_print(StateApx* self) {
    // This is useful for debugging.
}

uint32_t wnd_bit_count_apx_next(StateApx* self, bool item) {
    // TODO: Fill me.
    self->now++;
    if (item == 1) {
        self->buckets[0].increment = 1;
        self->buckets[0].timestamp = self->now;
        self->count++;
    }
    int idx = 1;
    for (int i = 0; i < self->num_buckets; i++) {
        if (self->buckets[i].increment) {
            if (self->buckets[i].size == 1) {
                self->buckets[i].size = 0;
                self->buckets[i].timestamp = 0;
                self->buckets[i + 1].increment = 1;
                self->buckets[i + 1].timestamp = self->buckets[i].timestamp;
            } else {
                self->buckets[i].size = 1;
            }
            self->buckets[i].increment = 0;
        }
        if (self->now - self->buckets[i].timestamp >= self->wnd_size) {
            self->count -= self->buckets[i].size * idx;
            self->buckets[i].size = 0;
            self->buckets[i].timestamp = 0;
        }
        idx *= 2;
    }

    return self->count;
}

#endif // _WINDOW_BIT_COUNT_APX_
