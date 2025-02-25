#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include "window-bit-count.h"

#define W 10 // window size
#define N 100 // stream length

int main() {
    printf("**** TEST: Bit counting over a sliding window *****\n");

    State state;
    wnd_bit_count_new(&state, W);
    wnd_bit_count_print(&state);

    uint32_t last_output = 0;
    for (uint32_t i=1; i<=N; i++) {
        bool item = i % 2;
        last_output = wnd_bit_count_next(&state, item);
        wnd_bit_count_print(&state);
        printf("last output = %u\n", last_output);
    }
    printf("last output = %u\n", last_output);

    wnd_bit_count_destruct(&state);

    return 0;
}