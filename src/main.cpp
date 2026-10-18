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

// Temporary key bindings until the UI milestone: 1-6 pick an element, 0 picks the eraser.
void handleElementKeys(ElementId& selected) {
    if (IsKeyPressed(KEY_ONE)) selected = ElementId::Sand;
    if (IsKeyPressed(KEY_TWO)) selected = ElementId::Water;
    if (IsKeyPressed(KEY_THREE)) selected = ElementId::Stone;
    if (IsKeyPressed(KEY_FOUR)) selected = ElementId::Wood;
    if (IsKeyPressed(KEY_FIVE)) selected = ElementId::Fire;
    if (IsKeyPressed(KEY_SIX)) selected = ElementId::Smoke;
    if (IsKeyPressed(KEY_ZERO)) selected = ElementId::Empty;
}

}  // namespace

int main() {
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(kGridWidth * kPixelScale, kGridHeight * kPixelScale, "Falling Sand");
    SetTargetFPS(60);

    {  // scope so the Renderer releases its texture before the window closes
        Grid grid(kGridWidth, kGridHeight);
        Renderer renderer(grid);
        ElementId selected = ElementId::Sand;

        while (!WindowShouldClose()) {
            handleElementKeys(selected);

            // Left button paints the selected element; right button always erases.
            const bool painting = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
            const bool erasing = IsMouseButtonDown(MOUSE_BUTTON_RIGHT);
            if (painting || erasing) {
                int x = 0, y = 0;
                mouseToGrid(grid, x, y);
                grid.paintCircle(x, y, kBrushRadius, erasing ? ElementId::Empty : selected);
            }

            grid.step();

            BeginDrawing();
            ClearBackground(BLACK);
            const Rectangle screen{0, 0, static_cast<float>(GetScreenWidth()),
                                   static_cast<float>(GetScreenHeight())};
            renderer.draw(grid, screen);
            const char* name = selected == ElementId::Empty ? "Eraser" : elementDef(selected).name;
            DrawText(name, 10, 10, 20, RAYWHITE);
            EndDrawing();
        }
    }

    CloseWindow();
    return 0;
}
