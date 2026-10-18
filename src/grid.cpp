#include "grid.h"

#include <utility>

Grid::Grid(int width, int height)
    : width_(width), height_(height), cells_(static_cast<size_t>(width) * height) {}

void Grid::set(int x, int y, ElementId type) {
    if (!inBounds(x, y)) return;
    Cell& c = cell(x, y);
    c.type = type;
    c.shade = static_cast<uint8_t>(random());
    c.life = 0;
    c.lastUpdate = frame_;
}

void Grid::paintCircle(int cx, int cy, int radius, ElementId type) {
    for (int dy = -radius; dy <= radius; ++dy) {
        for (int dx = -radius; dx <= radius; ++dx) {
            if (dx * dx + dy * dy > radius * radius) continue;
            if (isEmpty(cx + dx, cy + dy)) set(cx + dx, cy + dy, type);
        }
    }
}

void Grid::clear() {
    for (Cell& c : cells_) c = Cell{};
}

void Grid::step() {
    ++frame_;
    // Scan bottom-up so falling particles move at most once per frame, and pick
    // a random horizontal direction each frame so piles don't lean to one side.
    const bool leftToRight = coinFlip();
    for (int y = height_ - 1; y >= 0; --y) {
        for (int i = 0; i < width_; ++i) {
            const int x = leftToRight ? i : width_ - 1 - i;
            Cell& c = cell(x, y);
            if (c.type == ElementId::Empty || c.lastUpdate == frame_) continue;
            // Stamp before moving; the stamp travels with the particle, so it
            // won't be processed again if it lands in a row we haven't scanned yet.
            c.lastUpdate = frame_;

            switch (elementDef(c.type).movement) {
                case Movement::None: break;
                case Movement::Powder: updatePowder(x, y); break;
            }
        }
    }
}

bool Grid::tryMove(int x, int y, int toX, int toY) {
    if (!isEmpty(toX, toY)) return false;
    std::swap(cell(x, y), cell(toX, toY));
    return true;
}

// Powder: fall straight down; if blocked, slide down-left or down-right
// (random side first). This is what makes sand form sloped piles.
void Grid::updatePowder(int x, int y) {
    if (tryMove(x, y, x, y + 1)) return;
    const int dir = coinFlip() ? 1 : -1;
    if (tryMove(x, y, x + dir, y + 1)) return;
    tryMove(x, y, x - dir, y + 1);
}

// xorshift32: fast, deterministic, and plenty random for a sandbox.
uint32_t Grid::random() {
    uint32_t s = rngState_;
    s ^= s << 13;
    s ^= s >> 17;
    s ^= s << 5;
    rngState_ = s;
    return s;
}
