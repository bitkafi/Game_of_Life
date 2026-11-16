#include "engine.h"
#include <stdlib.h>
#include <time.h>

void init_random(int rows, int cols, uint8_t *map) {
    srand((unsigned int)time(NULL));
    int stride = cols + 2;
    for (int r = 1; r <= rows; r++) {
        for (int c = 1; c <= cols; c++) {
            map[r * stride + c] = (rand() % 100 < 25) ? 1 : 0;
        }
    }
}

void next_generation(int rows, int cols, const uint8_t * restrict current, uint8_t * restrict next) {
    int stride = cols + 2;

    for (int r = 1; r <= rows; r++) {
        int row_offset = r * stride;
        for (int c = 1; c <= cols; c++) {
            int idx = row_offset + c;

            uint8_t neighbors =
                current[idx - stride - 1] + current[idx - stride] + current[idx - stride + 1] +
                current[idx - 1]          +                         current[idx + 1] +
                current[idx + stride - 1] + current[idx + stride] + current[idx + stride + 1];

            uint8_t alive = current[idx];
            next[idx] = (neighbors == 3 || (alive && neighbors == 2)) ? 1 : 0;
        }
    }
}