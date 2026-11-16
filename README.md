# Conway's Game of Life (C Engine + C# UI)

A data-oriented implementation of Conway's Game of Life. The simulation core is written in plain C with a cache-friendly memory layout and a branch-free inner loop, so the compiler can auto-vectorize it. The interactive desktop frontend is built in C# ([Raylib-cs](https://github.com/ChrisDill/Raylib-cs)) and reads the C engine's memory buffer directly through P/Invoke, without copying.

![Conway's Game of Life Screenshot](screenshot.png)

## Architecture & Optimizations

- **Decoupled core and frontend (`engine.c` / `GolUI`)**
  The simulation engine can be compiled as a shared library (`libgol.so`) and has no dependency on presentation code. It runs either inside a standalone ANSI terminal program or as the native backend of a graphical frontend.

- **1D flat memory layout**
  Both generation buffers are contiguous 1D byte arrays (`index = row * stride + col`), which keeps access patterns friendly to the CPU caches and hardware prefetcher.

- **Ghost border (halo padding)**
  The grid is allocated as `(rows + 2) * (cols + 2)` with a 1-cell dead border. Because the outer cells are always 0, the inner neighbor-counting loop needs no boundary checks. A branch-free loop is a good candidate for compiler auto-vectorization.

- **O(1) pointer swap**
  Moving to the next generation swaps the `current` and `next` pointers in constant time, with no `memcpy`.

- **Zero-copy C# P/Invoke bridge**
  The C# frontend allocates unmanaged memory (`NativeMemory.AllocZeroed`), passes the raw pointers to `libgol.so`, and renders straight from that buffer, so the grid never touches the managed heap or the garbage collector.

## Requirements

- Linux (the shared library is built as `libgol.so`)
- `gcc` (or a compatible C compiler)
- [.NET 8 SDK](https://dotnet.microsoft.com/download) for the graphical frontend (Raylib-cs is restored through NuGet)

## Build & Run

### Option 1: Graphical UI (C# + Raylib-cs)

Build the engine as a shared library and place it next to the UI project:

```bash
gcc -O3 -march=native -fPIC -shared engine.c -o GolUI/libgol.so
```

Run the frontend:

```bash
cd GolUI
dotnet run -c Release
```

**Controls**

| Input | Action |
| --- | --- |
| Left / Right click | Draw / erase live cells |
| `Space` | Pause / resume |
| `R` | Randomize the grid |
| `C` | Clear the grid |
| `Up` / `Down` | Increase / decrease simulation speed |

### Option 2: Standalone terminal version

```bash
gcc -O3 -march=native terminal.c engine.c -o app
./app
```

Press `Ctrl+C` to quit.

> **Note:** `-march=native` tunes the binary for the CPU it is built on. Binaries built this way may not run on other machines; drop the flag for a portable build.

## License

This project is licensed under the GNU General Public License v3.0. See the [LICENSE](LICENSE) file for details.
