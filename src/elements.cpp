#include "elements.h"

#include <iterator>

namespace {

// Indexed by ElementId; keep the order in sync with the enum.
constexpr ElementDef kElements[] = {
    // name    color                   jitter  movement
    {"Empty", {20, 20, 26, 255},       0,      Movement::None},
    {"Sand",  {219, 187, 110, 255},    24,     Movement::Powder},
};

static_assert(std::size(kElements) == static_cast<size_t>(ElementId::Count),
              "Every ElementId needs a row in kElements");

}  // namespace

const ElementDef& elementDef(ElementId id) {
    return kElements[static_cast<size_t>(id)];
}
