#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <assert.h>
#include "window-bit-count-apx.h"
#include "../window-bit-count/window-bit-count.h"

#define W 200 // window size
#define N 1000 // stream length
#define K 100 // relative error = 1 / K

int main() {
    printf("**** TEST: Bit counting over a sliding window (approximate) *****\n");

    double eps = 1.0 / K;

    State state;
    StateApx state_apx;
    uint32_t last_output = 0;
    uint32_t last_output_apx = 0;

    for (uint32_t wnd_sz=1; wnd_sz<=W; wnd_sz++) {
        wnd_bit_count_new(&state, W);
        wnd_bit_count_print(&state);

        wnd_bit_count_apx_new(&state_apx, W, K);
        // wnd_bit_count_apx_print(&state_apx);

        for (uint32_t i=1; i<=N; i++) {
            bool item = true; //i % 2;
            last_output = wnd_bit_count_next(&state, item);
            last_output_apx = wnd_bit_count_apx_next(&state_apx, item);
            // wnd_bit_count_apx_print(&state_apx);

            // printf("last output (precise) = %u\n", last_output);
            // printf("last output (approximate) = %u\n", last_output_apx);
            // printf("\n");
            if (last_output < last_output_apx) {
                wnd_bit_count_apx_print(&state_apx);
                printf("error: iteration %u\n", i);
                printf("last_output: %u, last_output_apx: %u\n", last_output, last_output_apx);
                printf("manual count: %u\n", manual_count(&state_apx));
            }
            assert(last_output >= last_output_apx);
            uint32_t error_abs = last_output - last_output_apx;
            if (K * error_abs > last_output) {
                wnd_bit_count_apx_print(&state_apx);
                printf("error: iteration %u\n", i);
                printf("K: %u, error_abs: %u, last_output: %u, last_output_apx: %u\n", K, error_abs, last_output, last_output_apx);
            }
            assert(K * error_abs <= last_output);
            //double error_rel = ((double) error_abs) / last_output;
            //printf("K = %u, eps = %lf: x = %u, x_est = %u, error_rel = %lf\n",
            //    K, eps, last_output, last_output_apx, error_rel);
        }

        wnd_bit_count_apx_destruct(&state_apx);
        wnd_bit_count_destruct(&state);
    }

    return 0;
}