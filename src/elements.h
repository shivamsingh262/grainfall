#pragma once

#include <cstdint>

#include "raylib.h"

// Every material in the sandbox. To add one: add an id here (before Count)
// and a matching row in the table in elements.cpp.
enum class ElementId : uint8_t {
    Empty,
    Sand,
    Water,
    Stone,
    Wood,
    Fire,
    Smoke,
    Ember,
    Count
};

// Broad movement class. The simulation implements one rule per class, so most
// new elements only need to pick a class and tune their properties.
enum class Movement : uint8_t {
    None,    // never moves on its own
    Powder,  // falls straight down, otherwise slides off diagonally
    Liquid,  // like powder, but also flows sideways to level out
    Gas,     // rises, drifting sideways when blocked
};

// Static description of an element. One row per ElementId; fields left out of
// a row keep the defaults below.
struct ElementDef {
    const char* name = "";
    Color color = {255, 0, 255, 255};  // base color
    uint8_t colorJitter = 0;           // max brightness variation per particle (0 = flat color)
    Movement movement = Movement::None;
    uint8_t density = 0;     // a moving element can push aside liquids/gases lighter than itself
    uint8_t dispersion = 0;  // liquids/gases: max cells moved sideways per frame

    // Lifetime: if lifetimeMax > 0, each particle lives a random number of
    // frames in [lifetimeMin, lifetimeMax] and then turns into `decaysInto`.
    uint8_t lifetimeMin = 0;
    uint8_t lifetimeMax = 0;
    ElementId decaysInto = ElementId::Empty;

    // Burning.
    uint8_t flammability = 0;              // chance (out of 256) per frame to catch fire next to a burning cell
    ElementId burnsInto = ElementId::Fire;  // what this becomes when it catches fire
    bool ignites = false;                  // sets flammable neighbors on fire
    bool extinguishes = false;             // puts out igniting neighbors (turning them into their decaysInto)

    // Emission: each frame, with chance emitChance/256, spawn `emits` in an empty cell above.
    ElementId emits = ElementId::Empty;
    uint8_t emitChance = 0;
};

const ElementDef& elementDef(ElementId id);
