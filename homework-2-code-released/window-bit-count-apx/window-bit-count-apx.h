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
    bool size;
    bool increment;
    uint32_t timestamp;
} Bucket;

typedef struct {
    uint32_t now;
    uint32_t k;
    uint32_t wnd_size;
    uint32_t num_buckets;
    uint32_t count;
    uint32_t b0_size;
    uint32_t b0_timestamp;
    Bucket *buckets;
    // TODO: Fill me.
} StateApx;


// k = 1/eps
// if eps = 0.01 (relative error 1%) then k = 100
// if eps = 0.001 (relative error 0.1%) the k = 1000
uint64_t wnd_bit_count_apx_new(StateApx* self, uint32_t wnd_size, uint32_t k) {
    assert(wnd_size >= 1);
    assert(k >= 1);

    // TODO: Fill me.

    uint32_t num_buckets = 0;
    uint32_t max_bits = k;

    while (max_bits <= wnd_size/2) {
        printf("Max bits: %u\n", max_bits);
        num_buckets++;
        max_bits *= 2;
    }

    printf("Number of buckets: %u\n", num_buckets);

    self->now = 0;
    self->k = k;
    self->wnd_size = wnd_size;
    self->num_buckets = num_buckets;
    self->count = 0;
    self->buckets = (Bucket*) malloc(num_buckets * sizeof(Bucket));
    self->b0_size = 0;
    self->b0_timestamp = 0;

    for (uint32_t i = 0; i < num_buckets; i++) {
        self->buckets[i].size = 0;
        self->buckets[i].increment = false;
        self->buckets[i].timestamp = 0;
    }

    printf("Number of buckets: %u, Window size: %u\n", self->num_buckets ,wnd_size);

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
    printf("Timestamp: %u, Count: %u\n", self->now, self->count);
    printf("B0 Size: %u, num buckets: %u\n", self->b0_size, self->num_buckets);
    for (int i = 0; i < self->num_buckets; i++) {
        printf("Bucket %u: Size: %u, Timestamp: %u\n", i, self->buckets[i].size, self->buckets[i].timestamp);
    }
}

uint32_t wnd_bit_count_apx_next(StateApx* self, bool item) {
    // TODO: Fill me.
    self->now++;
    int idx = self->k;
    // printf("Time: %u\n", self->now);
    for (int i = 0; i < self->num_buckets; i++) {
        if (self->now - self->buckets[i].timestamp >= self->wnd_size) {
            printf("Removing %u at %u: \n", self->buckets[i].size * idx, self->buckets[i].timestamp);
            self->count -= self->buckets[i].size * idx;
            self->buckets[i].size = 0;
        }
        if (self->buckets[i].increment) {
            if (i == 0) {
                // printf("B0_t: %u, Now: %u\n", self->b0_timestamp, self->now);
                self->buckets[i].timestamp = self->b0_timestamp + 1;
                self->b0_timestamp = self->now;
            } else {
                self->buckets[i].timestamp = self->buckets[i - 1].timestamp;
                self->buckets[i - 1].timestamp = 0;
            }
            if (self->buckets[i].size == 1) {
                self->buckets[i].size = 0;
                self->buckets[i + 1].increment = 1;
                self->buckets[i + 1].timestamp = self->buckets[i].timestamp;
            } else {
                self->buckets[i].size = 1;
            }
    
            self->buckets[i].increment = 0;
        }
        idx *= 2;
    }
    if (item == 1) {
        self->b0_size++;
        self->count++;
    }
    if (self->b0_size > self->k) {
        self->b0_size = 1;
        self->buckets[0].increment = 1;
    }

    return self->count;
}

#endif // _WINDOW_BIT_COUNT_APX_
