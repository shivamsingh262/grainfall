#pragma once

#include <cstdint>

#include "raylib.h"

// Every material in the sandbox. To add one: add an id here (before Count)
// and a matching row in the table in elements.cpp.
enum class ElementId : uint8_t {
    Empty,
    Sand,
    Count
};

// Broad movement class. The simulation implements one rule per class, so most
// new elements only need to pick a class and tune their properties.
enum class Movement : uint8_t {
    None,    // never moves on its own
    Powder,  // falls straight down, otherwise slides off diagonally
};

// Static description of an element. One row per ElementId.
struct ElementDef {
    const char* name;
    Color color;          // base color
    uint8_t colorJitter;  // max brightness variation per particle (0 = flat color)
    Movement movement;
};

const ElementDef& elementDef(ElementId id);
