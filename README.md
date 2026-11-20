# Vespera

A custom game engine built for high-performance VR, with a game built on top of it: neon wireframe on-rails shooter built for the Steam Frame
Some goals of the project:
- High performance with strict comfort thresholds (90Hz stable framerate)
- All C++ code is hand-written, with very few dependencies
- Cross-platform for windows and linux

Build instructions:
- Clone with submodules: `git clone --recursive https://github.com/grandmaster789/vespera`, or `git submodule update --init --recursive` afterwards.
- Windows: run from a "Developer PowerShell for VS 2022"; shaped-core's `ConfigureEnvironment` checks for it. Clang 19+ (the VS-bundled clang-cl works).
- Linux: Clang 19 or newer (CI uses 21). If the default `clang` is older, select a newer one in a git-ignored `CMakeUserPresets.json` that inherits a preset and sets `CMAKE_C_COMPILER`/`CMAKE_CXX_COMPILER`.
- Presets: `win-clang`, `linux-clang`, `linux-clang-asan`.
- Build and test: `cmake --preset <name> && cmake --build --preset <name> && ctest --preset <name>`
