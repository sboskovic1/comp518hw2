#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <time.h>

#include "../utils.h"
#include "window-bit-count.h"

#define W 1000000 // window size
#define N 1000000000 // stream length

int main() {
    char scratch[100];

    printf("**** BENCHMARK: Bit counting over a sliding window *****\n");

    u64_to_str_with_sep(N, ',', scratch);
    printf("stream length = %s\n", scratch);

    u64_to_str_with_sep(W, ',', scratch);
    printf("window size = %s\n", scratch);

    State state;
    uint64_t memory = wnd_bit_count_new(&state, W);

    struct timespec tick, tock;
	clock_gettime(CLOCK_MONOTONIC, &tick);

    uint32_t last_output = 0;
    for (uint32_t i=1; i<=N; i++) {
        bool item = i % 2;
        last_output = wnd_bit_count_next(&state, item);
    }

    clock_gettime(CLOCK_MONOTONIC, &tock);

    u64_to_str_with_sep(last_output, ',', scratch);
    printf("last output = %s\n", scratch);

	uint64_t duration_nano = 1000000000L * (tock.tv_sec - tick.tv_sec) + tock.tv_nsec - tick.tv_nsec;
    u64_to_str_with_sep(duration_nano, ',', scratch);
	printf("duration = %s nanoseconds\n", scratch);
	
	uint64_t throughput = (1000000000L * N) / duration_nano;
    u64_to_str_with_sep(throughput, ',', scratch);
	printf("throughput = %s items/sec\n", scratch);

    u64_to_str_with_sep(memory, ',', scratch);
    printf("memory footprint = %s bytes\n", scratch);

    wnd_bit_count_destruct(&state);

    return 0;
}