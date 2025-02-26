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

typedef struct Bucket {
    uint32_t timestamp;
    struct Bucket *next;
    struct Bucket *prev;
} Bucket;

typedef struct {
    uint32_t now;
    uint32_t k;
    uint32_t wnd_size;
    uint32_t num_buckets;
    uint32_t count;
    uint32_t maxSize;
    uint32_t categories;
    Bucket **headsFree;
    Bucket **headsFull;
    Bucket **tailsFull;
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
    uint32_t categories = 1;


    while (bits_stored < wnd_size) {
        num_buckets++;
        buckets++;
        bits_stored += bucket_size;
        if (buckets == k + 1) {
            categories++;
            buckets = 0;
            bucket_size *= 2;
        }
    }

    self->headsFree = (Bucket**) malloc(categories * sizeof(Bucket*));
    self->headsFull = (Bucket**) malloc(categories * sizeof(Bucket*));
    self->tailsFull = (Bucket**) malloc(categories * sizeof(Bucket*));
    self->buckets = (Bucket*) malloc(num_buckets * sizeof(Bucket));

    for (int i = 0; i < categories; i++) {
        for (int j = 0; j < k + 1 && i * (k + 1) + j < num_buckets; j++) {
            int idx = i * (k + 1) + j;
            self->buckets[idx].timestamp = 0;
            if (j == 0) {
                self->headsFree[i] = &self->buckets[idx];
                self->buckets[idx].next = &self->buckets[idx + 1];
                self->buckets[idx].prev = NULL;
            } else if (j == k) {
                self->buckets[idx].next = NULL;
                self->buckets[idx].prev = &self->buckets[idx - 1];
            } else {
                self->buckets[idx].next = &self->buckets[idx + 1];
                self->buckets[idx].prev = &self->buckets[idx - 1];
            }
        }
        self->headsFull[i] = NULL;
        self->tailsFull[i] = NULL;
    }


    self->maxSize = bucket_size;
    self->now = 0;
    self->k = k;
    self->wnd_size = wnd_size;
    self->num_buckets = num_buckets;
    self->count = 0;
    self->categories = categories;

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
        printf("Bucket %u: Timestamp: %u\n", i + 1,  self->buckets[i].timestamp);
    }
}

uint32_t wnd_bit_count_apx_next(StateApx* self, bool item) {

    struct Bucket **headsFree = self->headsFree;
    struct Bucket **headsFull = self->headsFull;
    struct Bucket **tailsFull = self->tailsFull;
    int categories = self->categories;

    self->now++;
    uint32_t merge_time = self->now;
    int size = 1;
    for (int i = 0; i < categories; i++) {
        if (headsFull[i] != NULL && self->now - headsFull[i]->timestamp >= self->wnd_size) {
            // printf("Removing size %u (bucket %u) at %u\n", size, i, self->now);
            self->count -= size;
            struct Bucket *tmp = headsFull[i];
            headsFull[i] = headsFull[i]->prev;
            if (headsFull[i] != NULL) {
                headsFull[i]->next = NULL;
            }
            tmp->timestamp = 0;
            tmp->next = headsFree[i];
            tmp->prev = NULL;
            headsFree[i] = tmp;
        } else if (headsFull[i] != NULL) {
            // printf("Remove at: %u\n", headsFull[i]->timestamp);
        }
        size *= 2;
    }

    if (item == 1) {
        self->count++;
        for (int i = 0; i < categories; i++) {
            if (headsFree[i] != NULL) {
                // Free bucket available in category, claim it and move it to highest timestamp full bucket
                Bucket *tmp = headsFree[i];
                tmp->timestamp = merge_time;
                headsFree[i] = tmp->next;
                if (tailsFull[i] == NULL) {
                    tailsFull[i] = tmp;
                    headsFull[i] = tmp;
                    tmp->next = NULL;
                    tmp->prev = NULL;
                } else {
                    tailsFull[i]-> prev = tmp;
                    tmp->next = tailsFull[i];
                    tmp->prev = NULL;
                    tailsFull[i] = tmp;
                }
                // printf("Placed size %u at %u\n", i + 1, self->now);
                break;
            } else {
                // No free bucket available in category, free up highest timestamp bucket
                // and merge it into next category
                N_MERGES++;
                merge_time = tailsFull[i]->timestamp;
                tailsFull[i]->timestamp = 0;
                Bucket *tmp = tailsFull[i];
                tailsFull[i] = tailsFull[i]->next;
                if (tailsFull[i] != NULL) {
                    tailsFull[i]->prev = NULL;
                }
                tmp->next = headsFree[i];
                headsFree[i] = tmp;
            }
        }
    }



//     // TODO: Fill me.
//     self->now++;
//     int size = 1;
//     int bucket = 0;
//     int size_category = 0;
//     bool placed = false;
//     int k = self->k;
//     int num_buckets = self->num_buckets;
//     int newest_idx = -1;
//     uint32_t newest_time = 0;
//     uint32_t prev_time = 0;
//     while (size <= self->maxSize) {
//         for (int i = size_category; i < k + 1 && i + k * size_category < num_buckets; i++) {
//             if (self->buckets[i + k * size_category].timestamp != 0 && self->now - self->buckets[i + k * size_category].timestamp >= self->wnd_size) {
//                 // printf("Removing bucket %u: %u - %u > %u\n", i + k * size_category, self->now, self->buckets[i + k * size_category].timestamp, self->count);
//                 self->count -= self->buckets[i + k * size_category].size * size;
//                 self->buckets[i + k * size_category].size = 0;
//                 self->buckets[i + k * size_category].timestamp = 0;
//             }
//             if (item == 1 && !placed) {
//                 if (self->buckets[i + k * size_category].size == 0) {
//                     // printf("placed in bucket %u at size %u\n", i + k * size_category, size);
//                     self->buckets[i + k * size_category].size = 1;
//                     if (size_category == 0) {
//                         self->buckets[i + k * size_category].timestamp = self->now;
//                     } else {
//                         self->buckets[i + k * size_category].timestamp = prev_time;
//                     }
//                     placed = true;
//                 } else if (newest_idx == -1 || self->buckets[i + k * size_category].timestamp > newest_time) {
//                     newest_time = self->buckets[i + k * size_category].timestamp;
//                     newest_idx = i + k * size_category;
//                 }    
//             }
//         }
//         if (!placed) {
//             prev_time = newest_time;
//             self->buckets[newest_idx].size = 0;
//             self->buckets[newest_idx].timestamp = 0;
//             newest_idx = -1;
//             newest_time = 0;
//         }

//         size *= 2;
//         size_category++;
//     }
//     if (item == 1) {
//         self->count++;
//     }

    return self->count;
}

uint32_t manual_count(StateApx* self) {
    uint32_t count = 0;
    int size = 1;
    int bucket = 1;
    for (int i = 0; i < self->num_buckets; i++) {

        count += size;
        bucket++;
        if (bucket > self->k + 1) {
            bucket = 1;
            size *= 2;
        }
    }
    return count;
}

void manual_count_by_bucket(StateApx* self) {
    printf("Manual count: \n");
    uint32_t count = 0;
    int size = 1;
    for (int i = 0; i < self->categories; i++) {
        struct Bucket *tmp = self->headsFull[i];
        while (tmp != NULL) {
            count++;
            tmp = tmp->prev;
        }
        printf("Size %u: %i\n", i + 1, count);
        count = 0;
    }    
}

#endif // _WINDOW_BIT_COUNT_APX_
