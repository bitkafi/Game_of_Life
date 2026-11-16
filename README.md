# Conway's Game of Life (C Engine)

A low-level, cache-friendly implementation of Conway's Game of Life written in C, designed with data-oriented memory layouts and zero-copy generation transitions.

## Architecture & Optimizations

Instead of representing cells as heavyweight structs (`x`, `y`, `neighbor_count`, `state`), the simulation treats the grid purely as contiguous raw bytes (`uint8_t`), letting memory indices implicitly define coordinates.

* **1D Flat Memory Layout:** Both generation buffers are allocated as contiguous 1D blocks via `calloc` (`index = row * stride + col`). This maximizes CPU L1/L2 cache locality and enables hardware prefetching during row-major traversal.
* **Ghost Border (Halo Padding):** The allocated grid includes a 1-cell dead padding border (`(rows + 2) * (cols + 2)`). Because outer boundaries are guaranteed to be `0`, the inner simulation loop evaluates all 8 neighbors without conditional boundary checks (`if` branching), allowing compiler auto-vectorization (SIMD).
* **$\mathcal{O}(1)$ Pointer Swap:** Rather than copying buffers between generations with `memcpy`, the engine swaps the `current_gen` and `next_gen` pointers at the end of each tick—reducing state transition overhead to three register assignments.
* **Flicker-Free Terminal Rendering:** Uses ANSI escape sequences (`\033[H`, `\033[?25l`) to overwrite frames in place without invoking external `clear` system calls.

## Build & Run

Compile with aggressive optimization flags enabled:

```bash
gcc main.c -o app
./app