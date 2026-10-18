#include "raylib.h"

#include "grid.h"
#include "renderer.h"
#include "ui.h"

namespace {

constexpr int kGridWidth = 320;
constexpr int kGridHeight = 180;
constexpr int kPixelScale = 4;  // window pixels per cell

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
        UiState ui;

        while (!WindowShouldClose()) {
            handleUiInput(ui, grid);

            // Left button paints the selected element; right button always erases.
            const bool painting = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
            const bool erasing = IsMouseButtonDown(MOUSE_BUTTON_RIGHT);
            if ((painting || erasing) && !isMouseOverUi()) {
                int x = 0, y = 0;
                mouseToGrid(grid, x, y);
                grid.paintCircle(x, y, ui.brushRadius, erasing ? ElementId::Empty : ui.selected);
            }

            if (!ui.paused) grid.step();

            BeginDrawing();
            ClearBackground(BLACK);
            const Rectangle screen{0, 0, static_cast<float>(GetScreenWidth()),
                                   static_cast<float>(GetScreenHeight())};
            renderer.draw(grid, screen);
            drawUi(ui, grid);
            EndDrawing();
        }
    }

    CloseWindow();
    return 0;
}
