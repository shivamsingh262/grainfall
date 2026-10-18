#include "renderer.h"

#include <algorithm>

#include "grid.h"

namespace {

// Integer hash, used to give flickering cells a fresh pseudo-random shade
// each frame without touching simulation state.
uint32_t hash(uint32_t v) {
    v ^= v >> 16;
    v *= 0x7feb352du;
    v ^= v >> 15;
    v *= 0x846ca68bu;
    v ^= v >> 16;
    return v;
}

uint8_t clampChannel(int v) {
    return static_cast<uint8_t>(std::clamp(v, 0, 255));
}

}  // namespace

Renderer::Renderer(const Grid& grid)
    : pixels_(static_cast<size_t>(grid.width()) * grid.height()) {
    Image image = GenImageColor(grid.width(), grid.height(), BLACK);
    ImageFormat(&image, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);  // matches the Color layout of pixels_
    texture_ = LoadTextureFromImage(image);
    UnloadImage(image);
    SetTextureFilter(texture_, TEXTURE_FILTER_POINT);  // crisp pixels when scaled up
}

Renderer::~Renderer() {
    UnloadTexture(texture_);
}

// Base element color, brightened or darkened by the cell's shade (0-255 maps
// to -jitter..+jitter) so piles look grainy rather than flat.
Color Renderer::cellColor(const Grid& grid, int x, int y) const {
    const Cell& c = grid.at(x, y);
    const ElementDef& def = elementDef(c.type);
    if (def.colorJitter == 0) return def.color;

    const uint32_t shade =
        def.flickers ? hash(static_cast<uint32_t>(y * grid.width() + x) ^ hash(frame_)) & 0xFFu : c.shade;
    const int offset = (static_cast<int>(shade) - 128) * def.colorJitter / 128;
    return {clampChannel(def.color.r + offset), clampChannel(def.color.g + offset),
            clampChannel(def.color.b + offset), def.color.a};
}

void Renderer::draw(const Grid& grid, Rectangle dest) {
    for (int y = 0; y < grid.height(); ++y) {
        for (int x = 0; x < grid.width(); ++x) {
            pixels_[y * grid.width() + x] = cellColor(grid, x, y);
        }
    }
    UpdateTexture(texture_, pixels_.data());
    ++frame_;

    const Rectangle src{0, 0, static_cast<float>(grid.width()), static_cast<float>(grid.height())};
    DrawTexturePro(texture_, src, dest, {0, 0}, 0.0f, WHITE);
}
