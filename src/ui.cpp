#include "ui.h"

#include <algorithm>

#include "grid.h"
#include "raylib.h"

namespace {

constexpr int kMaxBrushRadius = 30;

// Palette layout: one button per element, eraser first. Built straight from
// the element table, so new elements show up here automatically.
constexpr int kButtonWidth = 84;
constexpr int kButtonHeight = 26;
constexpr int kButtonGap = 4;
constexpr int kMargin = 10;
constexpr int kFontSize = 16;
constexpr int kPaletteSize = static_cast<int>(ElementId::Count);  // eraser + every non-empty element

// Slot 0 is the eraser (Empty); slot i is ElementId i.
ElementId paletteElement(int slot) {
    return static_cast<ElementId>(slot);
}

const char* paletteName(int slot) {
    return slot == 0 ? "Eraser" : elementDef(paletteElement(slot)).name;
}

Rectangle paletteButton(int slot) {
    return {static_cast<float>(kMargin + slot * (kButtonWidth + kButtonGap)), static_cast<float>(kMargin),
            static_cast<float>(kButtonWidth), static_cast<float>(kButtonHeight)};
}

// Black text on light swatches, white on dark ones.
Color labelColorFor(Color bg) {
    const float luminance = 0.299f * bg.r + 0.587f * bg.g + 0.114f * bg.b;
    return luminance > 140.0f ? BLACK : RAYWHITE;
}

void drawPalette(const UiState& ui) {
    for (int slot = 0; slot < kPaletteSize; ++slot) {
        const Rectangle r = paletteButton(slot);
        const Color bg = slot == 0 ? Color{55, 55, 62, 255} : elementDef(paletteElement(slot)).color;
        DrawRectangleRec(r, bg);

        // Show the number key for the first ten slots (0 = eraser, 1-9 = elements).
        const char* label = slot <= 9 ? TextFormat("%d %s", slot, paletteName(slot)) : paletteName(slot);
        const int textX = static_cast<int>(r.x + (r.width - MeasureText(label, kFontSize)) / 2);
        DrawText(label, textX, static_cast<int>(r.y) + 5, kFontSize, labelColorFor(bg));

        const bool selected = paletteElement(slot) == ui.selected;
        DrawRectangleLinesEx(r, selected ? 3.0f : 1.0f, selected ? RAYWHITE : Color{0, 0, 0, 160});
    }
}

void drawHud(const UiState& ui) {
    const int right = GetScreenWidth() - kMargin;
    const char* fps = TextFormat("%d FPS", GetFPS());
    DrawText(fps, right - MeasureText(fps, 20), kMargin, 20, LIME);
    const char* brush = TextFormat("Brush %d", ui.brushRadius);
    DrawText(brush, right - MeasureText(brush, kFontSize), kMargin + 24, kFontSize, RAYWHITE);
    if (ui.paused) DrawText("PAUSED", right - MeasureText("PAUSED", 20), kMargin + 44, 20, YELLOW);

    const char* help = "LMB paint   RMB erase   Wheel brush size   Space pause   C clear";
    const int y = GetScreenHeight() - kMargin - kFontSize;
    DrawRectangle(0, y - 6, MeasureText(help, kFontSize) + 2 * kMargin, kFontSize + 12, Color{0, 0, 0, 120});
    DrawText(help, kMargin, y, kFontSize, Color{220, 220, 220, 255});
}

// Outline of the area the brush will paint, so the size is visible.
void drawBrushOutline(const UiState& ui, const Grid& grid) {
    if (isMouseOverUi()) return;
    const float cellSize = static_cast<float>(GetScreenWidth()) / grid.width();
    DrawCircleLinesV(GetMousePosition(), (ui.brushRadius + 0.5f) * cellSize, Color{255, 255, 255, 140});
}

}  // namespace

void handleUiInput(UiState& ui, Grid& grid) {
    // Number keys: 0 = eraser, 1-9 = elements in table order.
    for (int slot = 0; slot < std::min(kPaletteSize, 10); ++slot) {
        if (IsKeyPressed(KEY_ZERO + slot)) ui.selected = paletteElement(slot);
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        for (int slot = 0; slot < kPaletteSize; ++slot) {
            if (CheckCollisionPointRec(GetMousePosition(), paletteButton(slot))) ui.selected = paletteElement(slot);
        }
    }

    // One step per wheel event; trackpads send fractional values, so only the sign matters.
    const float wheel = GetMouseWheelMove();
    if (wheel > 0) ui.brushRadius = std::min(ui.brushRadius + 1, kMaxBrushRadius);
    if (wheel < 0) ui.brushRadius = std::max(ui.brushRadius - 1, 0);

    if (IsKeyPressed(KEY_SPACE)) ui.paused = !ui.paused;
    if (IsKeyPressed(KEY_C)) grid.clear();
}

bool isMouseOverUi() {
    const Rectangle palette = {static_cast<float>(kMargin), static_cast<float>(kMargin),
                               static_cast<float>(kPaletteSize * (kButtonWidth + kButtonGap)),
                               static_cast<float>(kButtonHeight)};
    return CheckCollisionPointRec(GetMousePosition(), palette);
}

void drawUi(const UiState& ui, const Grid& grid) {
    drawPalette(ui);
    drawHud(ui);
    drawBrushOutline(ui, grid);
}
