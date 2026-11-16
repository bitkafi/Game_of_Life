using System;
using System.Runtime.InteropServices;
using Raylib_cs;

unsafe class Program
{
    // C kütüphanendeki fonksiyon imzaları
    [DllImport("libgol.so", CallingConvention = CallingConvention.Cdecl)]
    static extern void init_random(int rows, int cols, byte* map);

    [DllImport("libgol.so", CallingConvention = CallingConvention.Cdecl)]
    static extern void next_generation(int rows, int cols, byte* current, byte* next);

    static void Main()
    {
        const int rows = 80;
        const int cols = 120;
        const int cellSize = 10;
        int stride = cols + 2;
        nuint totalBytes = (nuint)((rows + 2) * stride);

        // C motorunla uyumlu, hizalı ve sıfırlanmış native bellek tahsisi
        byte* currentGen = (byte*)NativeMemory.AllocZeroed(totalBytes);
        byte* nextGen    = (byte*)NativeMemory.AllocZeroed(totalBytes);

        // C fonksiyonunu çağırarak rastgele harita oluştur
        init_random(rows, cols, currentGen);

        Raylib.InitWindow(cols * cellSize, rows * cellSize + 40, "Conway's Game of Life - C Engine + C# UI");
        Raylib.SetTargetFPS(60);

        bool paused = false;
        int generation = 0;
        double timer = 0.0;
        double stepInterval = 0.05; // 50ms (saniyede 20 nesil)

        while (!Raylib.WindowShouldClose())
        {
            // Klavye Kontrolleri
            if (Raylib.IsKeyPressed(KeyboardKey.Space)) paused = !paused;
            if (Raylib.IsKeyPressed(KeyboardKey.R))
            {
                NativeMemory.Clear(currentGen, totalBytes);
                init_random(rows, cols, currentGen);
                generation = 0;
            }
            if (Raylib.IsKeyPressed(KeyboardKey.C))
            {
                NativeMemory.Clear(currentGen, totalBytes);
                generation = 0;
            }
            if (Raylib.IsKeyPressed(KeyboardKey.Up))   stepInterval = Math.Max(0.01, stepInterval - 0.01);
            if (Raylib.IsKeyPressed(KeyboardKey.Down)) stepInterval = Math.Min(0.50, stepInterval + 0.01);

            // Fare ile canlı/ölü hücre çizme
            if (Raylib.IsMouseButtonDown(MouseButton.Left) || Raylib.IsMouseButtonDown(MouseButton.Right))
            {
                int mc = Raylib.GetMouseX() / cellSize + 1;
                int mr = Raylib.GetMouseY() / cellSize + 1;

                if (mr >= 1 && mr <= rows && mc >= 1 && mc <= cols)
                {
                    byte val = Raylib.IsMouseButtonDown(MouseButton.Left) ? (byte)1 : (byte)0;
                    currentGen[mr * stride + mc] = val;
                }
            }

            // Simülasyon Adımı (C Motorunu Çağır + Pointer Swap)
            if (!paused)
            {
                timer += Raylib.GetFrameTime();
                if (timer >= stepInterval)
                {
                    timer = 0.0;
                    next_generation(rows, cols, currentGen, nextGen);

                    byte* temp = currentGen;
                    currentGen = nextGen;
                    nextGen = temp;
                    generation++;
                }
            }

            // Çizim (Render)
            Raylib.BeginDrawing();
            Raylib.ClearBackground(new Color(18, 18, 22, 255));

            for (int r = 1; r <= rows; r++)
            {
                int rowOffset = r * stride;
                for (int c = 1; c <= cols; c++)
                {
                    if (currentGen[rowOffset + c] == 1)
                    {
                        Raylib.DrawRectangle(
                            (c - 1) * cellSize,
                            (r - 1) * cellSize,
                            cellSize - 1,
                            cellSize - 1,
                            new Color(80, 220, 120, 255)
                        );
                    }
                }
            }

            // Alt Bilgi Çubuğu
            string status = paused ? "PAUSED" : "RUNNING";
            Raylib.DrawText(
                $"Gen: {generation} | {status} | [SPACE] Durdur  [R] Rastgele  [C] Temizle  [Sol/Sag Tik] Ciz/Sil  [Yukari/Asagi] Hiz",
                10, rows * cellSize + 12, 16, Color.LightGray
            );

            Raylib.EndDrawing();
        }

        Raylib.CloseWindow();
        NativeMemory.Free(currentGen);
        NativeMemory.Free(nextGen);
    }
}