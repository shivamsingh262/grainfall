#pragma once

#include <cstdint>
#include <vector>

#include "raylib.h"

class Grid;

// Draws the whole grid with a single texture: cells are written into a CPU
// pixel buffer, uploaded once per frame, and drawn scaled up to `dest`.
class Renderer {
public:
    explicit Renderer(const Grid& grid);  // requires an open window
    ~Renderer();
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    void draw(const Grid& grid, Rectangle dest);

private:
    Color cellColor(const Grid& grid, int x, int y) const;

    Texture2D texture_{};
    uint32_t frame_ = 0;  // drives flicker
    std::vector<Color> pixels_;
};
