#pragma once

#include <cstdint>
#include <vector>

#include "elements.h"

struct Cell {
    ElementId type = ElementId::Empty;
    uint8_t shade = 0;       // random per-particle color variation, rolled on spawn
    uint8_t life = 0;        // remaining lifetime for transient elements
    uint8_t lastUpdate = 0;  // frame stamp; equals the current frame once this cell has been updated
};

// Fixed-size cellular automaton. Owns the cells and advances the simulation.
class Grid {
public:
    Grid(int width, int height);

    int width() const { return width_; }
    int height() const { return height_; }
    bool inBounds(int x, int y) const { return x >= 0 && x < width_ && y >= 0 && y < height_; }
    const Cell& at(int x, int y) const { return cells_[y * width_ + x]; }

    // Spawns a fresh particle (overwrites whatever is there).
    void set(int x, int y, ElementId type);
    // Paints a filled circle of `type` into empty cells.
    void paintCircle(int cx, int cy, int radius, ElementId type);
    void clear();

    // Advances the simulation by one tick.
    void step();

private:
    Cell& cell(int x, int y) { return cells_[y * width_ + x]; }
    bool isEmpty(int x, int y) const { return inBounds(x, y) && at(x, y).type == ElementId::Empty; }
    bool tryMove(int x, int y, int toX, int toY);

    void updatePowder(int x, int y);

    uint32_t random();
    bool coinFlip() { return random() & 1u; }

    int width_;
    int height_;
    std::vector<Cell> cells_;
    uint8_t frame_ = 0;
    uint32_t rngState_ = 0x9E3779B9u;
};
