#include "raylib.h"

#include "grid.h"
#include "renderer.h"
#include "ui.h"

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

namespace {

constexpr int kGridWidth = 320;
constexpr int kGridHeight = 180;
constexpr int kPixelScale = 4;  // window pixels per cell

// Everything the frame loop reads and writes, so one frame can run as a callback
// (the browser drives the loop on the web instead of a blocking while).
struct App {
    Grid grid{kGridWidth, kGridHeight};
    Renderer renderer{grid};
    UiState ui;
    bool stroking = false;  // true while a paint/erase stroke is in progress
    int lastX = 0, lastY = 0;  // grid position painted last frame, to connect fast strokes
};

// Converts a window-space mouse position to grid cell coordinates.
void mouseToGrid(const Grid& grid, int& outX, int& outY) {
    const Vector2 mouse = GetMousePosition();
    outX = static_cast<int>(mouse.x * grid.width() / GetScreenWidth());
    outY = static_cast<int>(mouse.y * grid.height() / GetScreenHeight());
}

void updateDrawFrame(void* arg) {
    App& app = *static_cast<App*>(arg);
    Grid& grid = app.grid;
    handleUiInput(app.ui, grid);

    // Left button paints the selected element; right button always erases.
    const bool painting = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    const bool erasing = IsMouseButtonDown(MOUSE_BUTTON_RIGHT);
    if ((painting || erasing) && !isMouseOverUi()) {
        int x = 0, y = 0;
        mouseToGrid(grid, x, y);
        if (!app.stroking) {  // new stroke: start the line at the current position
            app.lastX = x;
            app.lastY = y;
        }
        grid.paintLine(app.lastX, app.lastY, x, y, app.ui.brushRadius, erasing ? ElementId::Empty : app.ui.selected);
        app.lastX = x;
        app.lastY = y;
        app.stroking = true;
    } else {
        app.stroking = false;
    }

    if (!app.ui.paused) grid.step();

    BeginDrawing();
    ClearBackground(BLACK);
    const Rectangle screen{0, 0, static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())};
    app.renderer.draw(grid, screen);
    drawUi(app.ui, grid);
    EndDrawing();
}

}  // namespace

int main() {
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(kGridWidth * kPixelScale, kGridHeight * kPixelScale, "Grainfall");

    {  // scope so the Renderer releases its texture before the window closes
        App app;
#if defined(PLATFORM_WEB)
        // 0 fps = follow requestAnimationFrame; never returns.
        emscripten_set_main_loop_arg(updateDrawFrame, &app, 0, 1);
#else
        SetTargetFPS(60);
        while (!WindowShouldClose()) updateDrawFrame(&app);
#endif
    }

    CloseWindow();
    return 0;
}
