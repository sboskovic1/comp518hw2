#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <time.h>

#include "../utils.h"
#include "window-bit-count-apx.h"

#define W 100000000 // window size
#define N 1000000000 // stream length
#define K 1000 // relative error = 1 / K

int main() {
    char scratch[100];

    printf("**** BENCHMARK: Bit counting over a sliding window (approximate) *****\n");

    N_MERGES = 0;

    u64_to_str_with_sep(N, ',', scratch);
    printf("stream length = %s\n", scratch);

    u64_to_str_with_sep(W, ',', scratch);
    printf("window size = %s\n", scratch);

    u64_to_str_with_sep(K, ',', scratch);
    printf("k = %s\n", scratch);

    StateApx state;
    uint64_t memory = wnd_bit_count_apx_new(&state, W, K);

    struct timespec tick, tock;
	clock_gettime(CLOCK_MONOTONIC, &tick);

    uint32_t last_output = 0;
    for (uint32_t i=1; i<=N; i++) {
        bool item = true; //i % 2;
        last_output = wnd_bit_count_apx_next(&state, item);
    }

    clock_gettime(CLOCK_MONOTONIC, &tock);

    u64_to_str_with_sep(last_output, ',', scratch);
    printf("last output = %s\n", scratch);

    u64_to_str_with_sep(N_MERGES, ',', scratch);
    printf("number of merges = %s\n", scratch);

	uint64_t duration_nano = 1000000000L * (tock.tv_sec - tick.tv_sec) + tock.tv_nsec - tick.tv_nsec;
    u64_to_str_with_sep(duration_nano, ',', scratch);
	printf("duration = %s nanoseconds\n", scratch);
	
	uint64_t throughput = (1000000000L * N) / duration_nano;
    u64_to_str_with_sep(throughput, ',', scratch);
	printf("throughput = %s items/sec\n", scratch);

    u64_to_str_with_sep(memory, ',', scratch);
    printf("memory footprint = %s bytes\n", scratch);

    wnd_bit_count_apx_destruct(&state);

    return 0;
}