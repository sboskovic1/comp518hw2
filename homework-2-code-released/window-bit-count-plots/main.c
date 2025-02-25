#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>

#include "../utils.h"
#include "../window-bit-count/window-bit-count.h"
#include "../window-bit-count-apx/window-bit-count-apx.h"

#define T 5 // number of trials per experiment
#define P 3 // pause in seconds
#define NUM_W 8
#define NUM_K 3

typedef struct {
	uint32_t algo;
	uint32_t wnd_sz;
	uint64_t throughput;
	uint64_t memory;
} Record;

const uint32_t N = 150*1000*1000L; // stream length
const uint32_t W_OPTIONS[NUM_W] = { // window sizes
	10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000
};
const uint32_t K_OPTIONS[NUM_K] = {
	// choices for k parameter (approximation algorithm)
	10, 100, 1000
};

uint32_t r_index = 0;
Record results[T*NUM_W*(1+NUM_K)];

void execute(uint32_t wnd_sz) {
    char scratch[100];

    printf("**** BENCHMARK: Bit counting over a sliding window *****\n");

    u64_to_str_with_sep(N, ',', scratch);
    printf("stream length = %s\n", scratch);

    u64_to_str_with_sep(wnd_sz, ',', scratch);
    printf("window size = %s\n", scratch);

    State state;
    uint64_t memory = wnd_bit_count_new(&state, wnd_sz);

    struct timespec tick, tock;
	clock_gettime(CLOCK_MONOTONIC, &tick);

    uint32_t last_output = 0;
    for (uint32_t i=1; i<=N; i++) {
        bool item = true; //i % 2;
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

    printf("\n");

    wnd_bit_count_destruct(&state);

	results[r_index].algo = 0;
	results[r_index].wnd_sz = wnd_sz;
	results[r_index].throughput = throughput;
	results[r_index].memory = memory;
    r_index += 1;
}

void execute_apx(uint32_t wnd_sz, uint32_t k_index) {
    char scratch[100];

    printf("**** BENCHMARK: Bit counting over a sliding window (approximate) *****\n");

    N_MERGES = 0;

    u64_to_str_with_sep(N, ',', scratch);
    printf("stream length = %s\n", scratch);

    u64_to_str_with_sep(wnd_sz, ',', scratch);
    printf("window size = %s\n", scratch);

    uint32_t k = K_OPTIONS[k_index];
    u64_to_str_with_sep(k, ',', scratch);
    printf("k = %s\n", scratch);

    StateApx state;
    uint64_t memory = wnd_bit_count_apx_new(&state, wnd_sz, k);

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

    printf("\n");

    wnd_bit_count_apx_destruct(&state);

	results[r_index].algo = k_index + 1;
	results[r_index].wnd_sz = wnd_sz;
	results[r_index].throughput = throughput;
	results[r_index].memory = memory;
    r_index += 1;
}

int main() {
    char scratch[100];

    printf("**** COMPARISON: Bit counting over a sliding window *****\n");
    printf("\n");

    State state;
    StateApx state_apx;

	for (uint32_t i=0; i<T; i++) {
		for (uint32_t j=0; j<NUM_W; j++) {
			uint32_t wnd_sz = W_OPTIONS[j];
            execute(wnd_sz);
            sleep(P);
            for (uint32_t k=0; k<NUM_K; k++) {
                execute_apx(wnd_sz, k);
                sleep(P);
            }
		}
	} // trial loop

	time_t now;
	time(&now);
	struct tm *local = localtime(&now);
	int hours = local->tm_hour;
	int minutes = local->tm_min;
	int seconds = local->tm_sec;
	int day = local->tm_mday;
	int month = local->tm_mon + 1;
	int year = local->tm_year + 1900;

    sprintf(scratch, "%04d_%02d_%02d_%02d_%02d_%02d_results.txt",
        year, month, day, hours, minutes, seconds);
    printf("output file name = %s\n", scratch);

    FILE* stream1 = fopen(scratch, "w");
	if (stream1 == NULL) {
		printf("%s could not be opened.\n", scratch);
		exit(1);
	}
    sprintf(scratch, "%s", "results.txt");
    FILE* stream2 = fopen(scratch, "w");
	if (stream2 == NULL) {
		printf("%s could not be opened.\n", scratch);
		exit(1);
	}

    for (uint32_t i=0; i<r_index; i++) {
        Record r = results[i];
        if (r.algo == 0) {
            sprintf(scratch, "exact");
        } else {
            sprintf(scratch, "apx[k=%u]", K_OPTIONS[r.algo-1]);
        }
        char scratch2[200];
        sprintf(scratch2, "%s %u %lu %lu",
            scratch, r.wnd_sz, r.throughput, r.memory);
        printf("%s\n", scratch2);
        fprintf(stream1, "%s\n", scratch2);
        fprintf(stream2, "%s\n", scratch2);
    }

    fclose(stream1);
    fclose(stream2);

    return 0;
}
