#include "renderer.h"

#include "grid.h"

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

void Renderer::draw(const Grid& grid, Rectangle dest) {
    for (int y = 0; y < grid.height(); ++y) {
        for (int x = 0; x < grid.width(); ++x) {
            pixels_[y * grid.width() + x] = elementDef(grid.at(x, y).type).color;
        }
    }
    UpdateTexture(texture_, pixels_.data());

    const Rectangle src{0, 0, static_cast<float>(grid.width()), static_cast<float>(grid.height())};
    DrawTexturePro(texture_, src, dest, {0, 0}, 0.0f, WHITE);
}
