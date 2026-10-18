#pragma once

#include "elements.h"

class Grid;

// Everything the player controls besides the grid itself.
struct UiState {
    ElementId selected = ElementId::Sand;  // Empty means the eraser
    int brushRadius = 3;                   // in cells; 0 paints a single cell
    bool paused = false;
};

// Handles element keys, palette clicks, scroll-wheel brush size, pause and clear.
void handleUiInput(UiState& ui, Grid& grid);

// True when the mouse is over an on-screen control, so clicks there shouldn't paint.
bool isMouseOverUi();

// Draws the palette, HUD and brush outline on top of the grid.
void drawUi(const UiState& ui, const Grid& grid);
