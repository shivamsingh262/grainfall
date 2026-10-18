#include "raylib.h"

#include "grid.h"
#include "renderer.h"

namespace {

constexpr int kGridWidth = 320;
constexpr int kGridHeight = 180;
constexpr int kPixelScale = 4;  // window pixels per cell
constexpr int kBrushRadius = 3;

// Converts a window-space mouse position to grid cell coordinates.
void mouseToGrid(const Grid& grid, int& outX, int& outY) {
    const Vector2 mouse = GetMousePosition();
    outX = static_cast<int>(mouse.x * grid.width() / GetScreenWidth());
    outY = static_cast<int>(mouse.y * grid.height() / GetScreenHeight());
}

}  // namespace

int main() {
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(kGridWidth * kPixelScale, kGridHeight * kPixelScale, "Falling Sand");
    SetTargetFPS(60);

    {  // scope so the Renderer releases its texture before the window closes
        Grid grid(kGridWidth, kGridHeight);
        Renderer renderer(grid);

        while (!WindowShouldClose()) {
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                int x = 0, y = 0;
                mouseToGrid(grid, x, y);
                grid.paintCircle(x, y, kBrushRadius, ElementId::Sand);
            }

            grid.step();

            BeginDrawing();
            ClearBackground(BLACK);
            const Rectangle screen{0, 0, static_cast<float>(GetScreenWidth()),
                                   static_cast<float>(GetScreenHeight())};
            renderer.draw(grid, screen);
            EndDrawing();
        }
    }

    CloseWindow();
    return 0;
}
