#include "grid.h"

#include <utility>

Grid::Grid(int width, int height)
    : width_(width), height_(height), cells_(static_cast<size_t>(width) * height) {}

void Grid::set(int x, int y, ElementId type) {
    if (!inBounds(x, y)) return;
    Cell& c = cell(x, y);
    c.type = type;
    c.shade = static_cast<uint8_t>(random());
    const ElementDef& def = elementDef(type);
    c.life = def.lifetimeMax == 0 ? 0 : static_cast<uint8_t>(def.lifetimeMin + random() % (def.lifetimeMax - def.lifetimeMin + 1));
    c.lastUpdate = frame_;
}

void Grid::paintCircle(int cx, int cy, int radius, ElementId type) {
    for (int dy = -radius; dy <= radius; ++dy) {
        for (int dx = -radius; dx <= radius; ++dx) {
            if (dx * dx + dy * dy > radius * radius) continue;
            if (type == ElementId::Empty || isEmpty(cx + dx, cy + dy)) set(cx + dx, cy + dy, type);
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

            // If the particle burned out or was extinguished, the new one waits until next frame.
            if (updateLifetime(x, y)) continue;
            if (elementDef(c.type).ignites && updateBurning(x, y)) continue;
            updateEmission(x, y);

            switch (elementDef(c.type).movement) {
                case Movement::None: break;
                case Movement::Powder: updatePowder(x, y); break;
                case Movement::Liquid: updateLiquid(x, y); break;
                case Movement::Gas: updateGas(x, y); break;
            }
        }
    }
}

// A particle can move into empty space, or swap places with a lighter liquid
// or gas (so sand sinks through water and falls through smoke). Solids never
// get pushed around, and gases only rise into empty space.
bool Grid::canMoveInto(ElementId mover, int toX, int toY) const {
    if (!inBounds(toX, toY)) return false;
    const Cell& target = at(toX, toY);
    if (target.type == ElementId::Empty) return true;
    if (elementDef(mover).movement == Movement::Gas) return false;
    // A particle can be pushed aside only once per frame. Otherwise a falling
    // stream of sand would hand the same water cell upward grain by grain,
    // carrying it to the top of the stream in a single frame.
    if (target.lastDisplaced == frame_) return false;
    const ElementDef& targetDef = elementDef(target.type);
    const bool fluid = targetDef.movement == Movement::Liquid || targetDef.movement == Movement::Gas;
    return fluid && elementDef(mover).density > targetDef.density;
}

// Moves the particle at (x, y) to (toX, toY); whatever was there takes its place.
void Grid::swapCells(int x, int y, int toX, int toY) {
    std::swap(cell(x, y), cell(toX, toY));
    Cell& displaced = cell(x, y);
    if (displaced.type != ElementId::Empty) displaced.lastDisplaced = frame_;
}

bool Grid::tryMove(int x, int y, int toX, int toY) {
    if (!canMoveInto(at(x, y).type, toX, toY)) return false;
    swapCells(x, y, toX, toY);
    return true;
}

// Slides up to `reach` cells in direction `dir` along the current row,
// stopping at the first blocked cell.
bool Grid::tryFlowSideways(int x, int y, int dir, int reach) {
    const ElementId mover = at(x, y).type;
    int targetX = x;
    for (int i = 1; i <= reach && canMoveInto(mover, x + dir * i, y); ++i) targetX = x + dir * i;
    if (targetX == x) return false;
    swapCells(x, y, targetX, y);
    return true;
}

// Transient elements (fire, smoke) count down their life and turn into their
// decay product at zero. Returns true if the particle decayed.
bool Grid::updateLifetime(int x, int y) {
    Cell& c = cell(x, y);
    const ElementDef& def = elementDef(c.type);
    if (def.lifetimeMax == 0) return false;
    if (c.life > 0) {
        --c.life;
        return false;
    }
    set(x, y, def.decaysInto);
    return true;
}

// Burning cells check their 8 neighbors: an extinguishing neighbor (water)
// puts them out, otherwise each flammable neighbor may catch fire.
// Returns true if this cell was put out.
bool Grid::updateBurning(int x, int y) {
    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            const int nx = x + dx, ny = y + dy;
            if ((dx == 0 && dy == 0) || !inBounds(nx, ny)) continue;
            const ElementDef& neighbor = elementDef(at(nx, ny).type);
            if (neighbor.extinguishes) {
                set(x, y, elementDef(at(x, y).type).decaysInto);
                return true;
            }
            if (neighbor.flammability > 0 && chance(neighbor.flammability)) set(nx, ny, neighbor.burnsInto);
        }
    }
    return false;
}

// Emitters (embers) occasionally spawn a particle (fire) just above themselves.
void Grid::updateEmission(int x, int y) {
    const ElementDef& def = elementDef(at(x, y).type);
    if (def.emitChance == 0 || !chance(def.emitChance)) return;
    const int ex = x + static_cast<int>(random() % 3) - 1;
    if (isEmpty(ex, y - 1)) set(ex, y - 1, def.emits);
}

// Powder: fall straight down; if blocked, slide down-left or down-right
// (random side first). This is what makes sand form sloped piles.
void Grid::updatePowder(int x, int y) {
    if (tryMove(x, y, x, y + 1)) return;
    const int dir = coinFlip() ? 1 : -1;
    if (tryMove(x, y, x + dir, y + 1)) return;
    tryMove(x, y, x - dir, y + 1);
}

// Liquid: fall like a powder, and when nothing below is free, flow sideways
// (up to `dispersion` cells per frame) so pools spread out and level off.
void Grid::updateLiquid(int x, int y) {
    if (tryMove(x, y, x, y + 1)) return;
    const int dir = coinFlip() ? 1 : -1;
    if (tryMove(x, y, x + dir, y + 1)) return;
    if (tryMove(x, y, x - dir, y + 1)) return;
    const int reach = elementDef(at(x, y).type).dispersion;
    if (tryFlowSideways(x, y, dir, reach)) return;
    tryFlowSideways(x, y, -dir, reach);
}

// Gas: rise (with a random sideways wobble so it flickers and billows), and
// drift sideways when blocked so smoke spreads along ceilings.
void Grid::updateGas(int x, int y) {
    const int wobble = static_cast<int>(random() % 3) - 1;
    if (tryMove(x, y, x + wobble, y - 1)) return;
    if (tryMove(x, y, x, y - 1)) return;
    const int dir = coinFlip() ? 1 : -1;
    if (tryMove(x, y, x + dir, y - 1)) return;
    if (tryMove(x, y, x - dir, y - 1)) return;
    const int reach = elementDef(at(x, y).type).dispersion;
    if (tryFlowSideways(x, y, dir, reach)) return;
    tryFlowSideways(x, y, -dir, reach);
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
