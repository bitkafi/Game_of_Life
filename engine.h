#ifndef ENGINE_H
#define ENGINE_H

#include <stdint.h>

void init_random(int rows, int cols, uint8_t *map);
void next_generation(int rows, int cols, const uint8_t * restrict current, uint8_t * restrict next);

#endif