#include "elements.h"

#include <iterator>

namespace {

// Indexed by ElementId; keep the order in sync with the enum.
constexpr ElementDef kElements[] = {
    {.name = "Empty", .color = {20, 20, 26, 255}},
    {.name = "Sand",
     .color = {219, 187, 110, 255},
     .colorJitter = 24,
     .movement = Movement::Powder,
     .density = 150},
    {.name = "Water",
     .color = {48, 110, 220, 255},
     .colorJitter = 10,
     .movement = Movement::Liquid,
     .density = 100,
     .dispersion = 5,
     .extinguishes = true},
    {.name = "Stone",
     .color = {120, 120, 128, 255},
     .colorJitter = 18,
     .density = 255},
    {.name = "Wood",
     .color = {115, 74, 42, 255},
     .colorJitter = 16,
     .density = 255,
     .flammability = 8,
     .burnsInto = ElementId::Ember},
    {.name = "Fire",
     .color = {255, 120, 30, 255},
     .colorJitter = 40,
     .movement = Movement::Gas,
     .density = 1,
     .dispersion = 1,
     .lifetimeMin = 15,
     .lifetimeMax = 45,
     .decaysInto = ElementId::Smoke,
     .ignites = true},
    {.name = "Smoke",
     .color = {70, 70, 74, 255},
     .colorJitter = 12,
     .movement = Movement::Gas,
     .density = 2,
     .dispersion = 2,
     .lifetimeMin = 40,
     .lifetimeMax = 120},
    // Burning wood: stays put, spreads fire to neighbors, and throws flames upward.
    {.name = "Ember",
     .color = {230, 70, 20, 255},
     .colorJitter = 40,
     .density = 255,
     .lifetimeMin = 60,
     .lifetimeMax = 160,
     .decaysInto = ElementId::Smoke,
     .ignites = true,
     .emits = ElementId::Fire,
     .emitChance = 40},
};

static_assert(std::size(kElements) == static_cast<size_t>(ElementId::Count),
              "Every ElementId needs a row in kElements");

}  // namespace

const ElementDef& elementDef(ElementId id) {
    return kElements[static_cast<size_t>(id)];
}
