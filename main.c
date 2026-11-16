#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define MAX_HEIGHT 25
#define MAX_WIDTH 100

void init_random(int rows, int cols, uint8_t *map) {
    int stride = cols + 2;
    for (int r = 1; r <= rows; r++) {
        for (int c = 1; c <= cols; c++) {
            map[r * stride + c] = (rand() % 100 < 25) ? 1 : 0;
        }
    }
}

void render(int rows, int cols, const uint8_t *map) {
    int stride = cols + 2;
    for (int r = 1; r <= rows; r++) {
        for (int c = 1; c <= cols; c++) {
            if (map[r * stride + c] == 1) {
                printf("# ");
            } else {
                printf("  ");
            }
        }
        printf("\n");
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

int main() {
    srand(time(NULL));

    size_t total_cells = (size_t)(MAX_HEIGHT + 2) * (MAX_WIDTH + 2);
    uint8_t *current_gen = calloc(total_cells, sizeof(uint8_t));
    uint8_t *next_gen    = calloc(total_cells, sizeof(uint8_t));

    if (!current_gen || !next_gen) {
        fprintf(stderr, "Bellek ayrilamadi!\n");
        return 1;
    }

    init_random(MAX_HEIGHT, MAX_WIDTH, current_gen);

    printf("\033[2J");
    printf("\033[?25l");

    while (1) {
        printf("\033[H");
        render(MAX_HEIGHT, MAX_WIDTH, current_gen);
        fflush(stdout);

        next_generation(MAX_HEIGHT, MAX_WIDTH, current_gen, next_gen);

        // Pointer swap
        uint8_t *temp = current_gen;
        current_gen = next_gen;
        next_gen = temp;

        usleep(100000);
    }

    free(current_gen);
    free(next_gen);
    return 0;
}