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
    uint32_t timestamp;
} Bucket;

typedef struct {
    uint32_t now;
    uint32_t k;
    uint32_t wnd_size;
    uint32_t num_buckets;
    uint32_t count;
    uint32_t maxSize;
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
    uint32_t bits_stored = 0;
    uint32_t buckets = 0;
    uint32_t bucket_size = 1;


    while (bits_stored < wnd_size) {
        num_buckets++;
        buckets++;
        bits_stored += bucket_size;
        if (buckets == k + 1) {
            buckets = 0;
            bucket_size *= 2;
        }
    }


    self->maxSize = bucket_size;
    self->now = 0;
    self->k = k;
    self->wnd_size = wnd_size;
    self->num_buckets = num_buckets;
    self->count = 0;
    self->buckets = (Bucket*) malloc(num_buckets * sizeof(Bucket));

    for (uint32_t i = 0; i < num_buckets; i++) {
        self->buckets[i].size = 0;
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
    printf("Timestamp: %u, Count: %u\n", self->now, self->count);
    printf("Num buckets: %u\n", self->num_buckets);
    for (int i = 0; i < self->num_buckets; i++) {
        printf("Bucket %u: Size: %u, Timestamp: %u\n", i + 1, self->buckets[i].size, self->buckets[i].timestamp);
    }
}

uint32_t wnd_bit_count_apx_next(StateApx* self, bool item) {
    // TODO: Fill me.
    self->now++;
    int size = 1;
    int bucket = 0;
    int size_category = 0;
    bool placed = false;
    int k = self->k;
    int num_buckets = self->num_buckets;
    int newest_idx = -1;
    uint32_t newest_time = 0;
    uint32_t prev_time = 0;
    while (size <= self->maxSize) {
        for (int i = size_category; i < k + 1 && i + k * size_category < num_buckets; i++) {
            if (self->buckets[i + k * size_category].timestamp != 0 && self->now - self->buckets[i + k * size_category].timestamp >= self->wnd_size) {
                // printf("Removing bucket %u: %u - %u > %u\n", i + k * size_category, self->now, self->buckets[i + k * size_category].timestamp, self->count);
                self->count -= self->buckets[i + k * size_category].size * size;
                self->buckets[i + k * size_category].size = 0;
                self->buckets[i + k * size_category].timestamp = 0;
            }
            if (item == 1 && !placed) {
                if (self->buckets[i + k * size_category].size == 0) {
                    // printf("placed in bucket %u at size %u\n", i + k * size_category, size);
                    self->buckets[i + k * size_category].size = 1;
                    if (size_category == 0) {
                        self->buckets[i + k * size_category].timestamp = self->now;
                    } else {
                        self->buckets[i + k * size_category].timestamp = prev_time;
                    }
                    placed = true;
                } else if (newest_idx == -1 || self->buckets[i + k * size_category].timestamp > newest_time) {
                    newest_time = self->buckets[i + k * size_category].timestamp;
                    newest_idx = i + k * size_category;
                }    
            }
        }
        if (!placed) {
            prev_time = newest_time;
            self->buckets[newest_idx].size = 0;
            self->buckets[newest_idx].timestamp = 0;
            newest_idx = -1;
            newest_time = 0;
        }

        size *= 2;
        size_category++;
    }
    if (item == 1) {
        self->count++;
    }

    return self->count;
}

uint32_t manual_count(StateApx* self) {
    uint32_t count = 0;
    int size = 1;
    int bucket = 1;
    for (int i = 0; i < self->num_buckets; i++) {

        count += self->buckets[i].size;
        bucket++;
        if (bucket == self->k + 1) {
            bucket = 1;
            size *= 2;
        }
    }
    return count;
}

#endif // _WINDOW_BIT_COUNT_APX_
