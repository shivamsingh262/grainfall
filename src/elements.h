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
    Count
};

// Broad movement class. The simulation implements one rule per class, so most
// new elements only need to pick a class and tune their properties.
enum class Movement : uint8_t {
    None,    // never moves on its own
    Powder,  // falls straight down, otherwise slides off diagonally
    Liquid,  // like powder, but also flows sideways to level out
};

// Static description of an element. One row per ElementId.
struct ElementDef {
    const char* name;
    Color color;          // base color
    uint8_t colorJitter;  // max brightness variation per particle (0 = flat color)
    Movement movement;
    uint8_t density;      // a moving element can push aside liquids lighter than itself
    uint8_t dispersion;   // liquids: max cells flowed sideways per frame
};

const ElementDef& elementDef(ElementId id);
