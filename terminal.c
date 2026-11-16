#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "engine.h"

#define MAX_HEIGHT 50
#define MAX_WIDTH 50

void render(int rows, int cols, const uint8_t *map) {
    int stride = cols + 2;
    for (int r = 1; r <= rows; r++) {
        for (int c = 1; c <= cols; c++) {
            printf(map[r * stride + c] == 1 ? "# " : "  ");
        }
        printf("\n");
    }
}

int main() {
    srand(time(NULL));

    size_t total_cells = (size_t)(MAX_HEIGHT + 2) * (MAX_WIDTH + 2);
    uint8_t *current_gen = calloc(total_cells, sizeof(uint8_t));
    uint8_t *next_gen    = calloc(total_cells, sizeof(uint8_t));

    if (!current_gen || !next_gen) {
        free(current_gen);
        free(next_gen);
        return 1;
    }

    init_random(MAX_HEIGHT, MAX_WIDTH, current_gen);

    printf("\033[2J\033[?25l");

    while (1) {
        printf("\033[H");
        render(MAX_HEIGHT, MAX_WIDTH, current_gen);
        fflush(stdout);

        next_generation(MAX_HEIGHT, MAX_WIDTH, current_gen, next_gen);

        uint8_t *temp = current_gen;
        current_gen = next_gen;
        next_gen = temp;

        usleep(100000);
    }

    free(current_gen);
    free(next_gen);
    return 0;
}