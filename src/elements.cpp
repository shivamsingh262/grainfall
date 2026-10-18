#include "elements.h"

#include <iterator>

namespace {

// Indexed by ElementId; keep the order in sync with the enum.
constexpr ElementDef kElements[] = {
    // name    color                   jitter  movement           density  dispersion
    {"Empty", {20, 20, 26, 255},       0,      Movement::None,    0,       0},
    {"Sand",  {219, 187, 110, 255},    24,     Movement::Powder,  150,     0},
    {"Water", {48, 110, 220, 255},     10,     Movement::Liquid,  100,     5},
    {"Stone", {120, 120, 128, 255},    18,     Movement::None,    255,     0},
};

static_assert(std::size(kElements) == static_cast<size_t>(ElementId::Count),
              "Every ElementId needs a row in kElements");

}  // namespace

const ElementDef& elementDef(ElementId id) {
    return kElements[static_cast<size_t>(id)];
}
