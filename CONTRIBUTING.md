# Contributing to Grainfall

Thanks for your interest! Grainfall aims to stay small and easy to read, so the best contributions are
focused and keep the code approachable.

## Getting started

1. Fork the repo and create a branch from `main`.
2. Build it (see the [README](README.md#building)):
   ```sh
   cmake -B build
   cmake --build build
   ```
3. Make your change, rebuild, and try it out in the sandbox.
4. Open a pull request describing what you changed and how you tested it. A screenshot or GIF is
   very welcome for anything visual.

CI builds every pull request on Linux, macOS and Windows, so please make sure it stays green.

## Good first contributions

- **New elements.** Follow [How to add an element](README.md#how-to-add-an-element). Ideas: oil,
  lava, acid, ice, steam, gunpowder, plants.
- **Tuning.** Colors, densities, lifetimes and burn rates are all in `src/elements.cpp`.
- **UI polish.** Tooltips, a help overlay, saving and loading scenes.

For larger changes (new systems, performance rewrites, new dependencies), please open an issue first
so we can agree on the approach.

## Code style

- C++20, raylib only. Please don't add other dependencies.
- Readable over clever. Prefer plain loops and small functions to dense tricks.
- Keep files focused: simulation rules in `grid.cpp`, element data in `elements.cpp`, drawing in
  `renderer.cpp`, controls in `ui.cpp`.
- Prefer data-driven behavior: add a property to `ElementDef` that any element can opt into rather
  than hard-coding a check for one element id.
- Write brief comments explaining *why* a simulation rule works the way it does.
- Match the surrounding formatting: 4-space indent, `camelCase` functions, `kConstants`, `member_`
  fields.
- No new compiler warnings (`-Wall -Wextra` / `/W4`).

## Reporting bugs

Open an issue with your OS, compiler, steps to reproduce, and what you expected to happen.
Screenshots help a lot with simulation bugs.

## License

By contributing, you agree that your contributions will be licensed under the [MIT License](LICENSE).
