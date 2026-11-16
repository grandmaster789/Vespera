# M0 Foundation Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.
>
> **For this project:** the owner implements the tasks (see the spec's
> "Working model"). The assistant explains each task before it starts and reviews
> it after.

**Goal:**
- Vespera builds on top of shaped-core.
- Where Vespera duplicates clean-core, it uses clean-core instead.
- The engine and the game are separate targets.
- The blog has its first post.

**Architecture:**
- shaped-core comes in as a git submodule in `extern/shaped-core`, added with
  `add_subdirectory(... EXCLUDE_FROM_ALL SYSTEM)`. `SC_LIBRARIES` limits it to what M0
  links.
- Vespera's assert and logger are deleted in favour of clean-core's `CC_ASSERT*` and
  `CC_LOG_*`.
- The arena stays, because clean-core has no bump allocator.
- The engine library becomes `vespera_engine`, and the game executable `vespera`
  moves to `game/vespera/`.

**Tech Stack:**
- C++23, CMake 3.28, Ninja; presets `win-clang`, `linux-clang` and
  `linux-clang-asan`. ARM64 runs `linux-clang` natively in CI.
- shaped-core `8c853df4752a8ec74b3fc212bcbe34d60de84374`: `clean-core` and
  `typed-geometry`.

**Spec:** `docs/superpowers/specs/2026-11-16-vespera-engine-roadmap-design.md`

## Global Constraints

- Warnings are errors on every Vespera target (`vespera_target_defaults`). shaped-core
  headers must not trip them, which is why the submodule is added with `SYSTEM`.
- shaped-core is pinned to commit `8c853df`. It is never modified in place; gaps
  are reported to its authors.
- Dependencies in M0: shaped-core only. No graphics libraries are linked yet
  (YAGNI: M2 adds them).
- Windows and Linux are equal. Every task passes CI: `win-clang`, `linux-clang-asan`
  on `ubuntu-26.04`, and `linux-clang` natively on `ubuntu-26.04-arm`.
- Linux needs **Clang 19 or newer** (CI uses 21); configure fails with a clear
  message otherwise. Locally, select the compiler with a git-ignored
  `CMakeUserPresets.json`, for example a preset that inherits `linux-clang-asan`
  and sets `CMAKE_C_COMPILER=clang-20` and `CMAKE_CXX_COMPILER=clang++-20`. Don't
  use `-D` overrides: a later plain re-configure silently drops them.
- Engine code (`src/`) never refers to the game (`game/`).

## Review Focus

1. **A fresh clone without `--recursive`.** The expected behaviour is that configure
   stops with a message naming `git submodule update --init`, not a cryptic
   missing-file error. This is tested in Task 1, Step 4.
2. **ASan preset with mimalloc.** LeakSanitizer can't see mimalloc memory, so
   clean-core containers report false leaks. `linux-clang-asan` must set
   `SC_MIMALLOC=OFF`. Covered in Task 1, Step 3.
3. **ARM64.** CI builds natively on `ubuntu-26.04-arm` (the qemu cross preset was
   dropped). A failure inside shaped-core is reported upstream, never patched
   inside the submodule. Covered in Task 1, Step 6.
4. **Death tests after the assert swap.** `cmake/death_test.cmake` matches
   `(Assertion|Check) failed:`. If clean-core words its failure message
   differently, the death test must still fail when nothing fires and pass when the
   assert fires. Covered in Task 2, Step 4.
5. **A Windows configure outside the Visual Studio developer environment.**
   shaped-core's `ConfigureEnvironment.cmake` refuses a build directory whose
   developer environment changes between configures. The expected behaviour is
   that a fresh `build/win-clang` configured from a VS developer prompt works, and
   the README says to use one. Covered in Task 3, Step 4.

---

### Task 1: Bring in shaped-core

**Files:**
- Create: `.gitmodules`, `extern/shaped-core` (submodule)
- Modify: `CMakeLists.txt` (top, before `project()`, and before the first target)
- Modify: `CMakePresets.json` (`linux-clang-asan`)
- Modify: `.github/workflows/ci.yml` (checkout step)
- Test: `tests/core/test_shaped_core.cpp`

**Interfaces:**
- Produces: CMake targets `clean-core` and `typed-geometry`, linked `PUBLIC` by
  `vespera`, so every test and the app see `<clean-core/...>` and
  `<typed-geometry/...>`.

- [ ] **Step 1: Write the failing smoke test** `tests/core/test_shaped_core.cpp`:

```cpp
#include <clean-core/container/vector.hh>
#include <typed-geometry/linalg/vec.hh>
#include <core/assert.h>

int main() {
    cc::vector<int> v;
    v.push_back(1);
    v.push_back(2);
    VESPERA_CHECK(v.size() == 2, "cc::vector must grow");

    tg::vec3f a(1, 2, 3);
    tg::vec3f b(4, 5, 6);
    VESPERA_CHECK((a + b)[0] == 5, "tg vector addition"); // tg has no .x members
}
```

The header paths and `tg::vec3f` (from `linalg/fwd.hh`) are confirmed at commit
`8c853df`.

- [ ] **Step 2: Configure and build. It should fail.**

Run: `cmake --preset win-clang && cmake --build --preset win-clang`
Expected: FAIL, because `clean-core/container/vector.hh` isn't found.

- [ ] **Step 3: Add the submodule and wire up CMake**
  - Add the submodule:
    `git submodule add https://github.com/solidean/shaped-core extern/shaped-core`,
    then `git -C extern/shaped-core checkout 8c853df4752a8ec74b3fc212bcbe34d60de84374`.
    Set `update = merge` in `.gitmodules`, as shaped-core's consumer guide asks.
  - In `CMakeLists.txt`, before `project()`:
    `include(extern/shaped-core/tools/cmake/ConfigureEnvironment.cmake)`. It must run
    before `project()` to work. Read the comment at the top of that file and put
    the gist in a one-line comment.
  - After `project()`, and before any Vespera target:
    - Add a `FATAL_ERROR` if `extern/shaped-core/CMakeLists.txt` doesn't exist. The
      message must name `git submodule update --init --recursive`.
    - Then:
      ```
      set(SC_LIBRARIES "clean-core;typed-geometry" CACHE STRING "")
      add_subdirectory(extern/shaped-core EXCLUDE_FROM_ALL SYSTEM)
      ```
  - Add `target_link_libraries(vespera PUBLIC clean-core typed-geometry)`.
  - In `CMakePresets.json`, add `"SC_MIMALLOC": "OFF"` to the `cacheVariables` of
    `linux-clang-asan`.
  - In `ci.yml`, add `with: { submodules: recursive }` to `actions/checkout@v4`.

- [ ] **Step 4: Verify the missing-submodule message**

Run: `git -C extern/shaped-core status` to confirm it's checked out. Then, in a
scratch clone made without `--recursive`, run `cmake --preset linux-clang` (or
`win-clang`).
Expected: configure stops with your message naming `git submodule update --init
--recursive`.

- [ ] **Step 5: Build and run the tests on the host preset**

Run: `cmake --preset win-clang && cmake --build --preset win-clang && ctest --preset win-clang`
Expected: every test passes, including `core_test_shaped_core`. There are no
warnings from shaped-core headers; if there are, `SYSTEM` didn't take effect.

- [ ] **Step 6: Run the Linux presets**

Run in WSL or a Linux machine: `cmake --preset linux-clang-asan && cmake --build
--preset linux-clang-asan && ctest --preset linux-clang-asan`, and let CI cover ARM64
natively.
Expected: all PASS. If ARM64 fails inside shaped-core, record the exact error in
`docs/blog/notes/m0.md` and report it upstream.

- [ ] **Step 7: Commit**

```bash
git add .gitmodules extern/shaped-core CMakeLists.txt CMakePresets.json .github/workflows/ci.yml tests/core/test_shaped_core.cpp
git commit -m "build: consume shaped-core (clean-core, typed-geometry) as a submodule"
```

---

### Task 2: Use clean-core's assert and logger instead of Vespera's

Why: clean-core already has assertions with per-thread handlers and logging over its
event stream. The spec says to keep a Vespera piece only where it does something
clean-core doesn't. The arena passes that test, because clean-core has no bump
allocator; the assert and the logger don't.

**Owner decision point:** if you want to keep your own assert or logger, for
example as blog material, say so at review. This task then shrinks to making
`VESPERA_ASSERT` a one-line alias for `CC_ASSERT`.

**Files:**
- Delete: `src/core/assert.h`, `src/core/assert.cpp`, `src/core/logger.h`,
  `src/core/logger.cpp`, `src/core/log_level.h`
- Delete: `tests/core/test_assert.cpp`, `tests/core/test_logger.cpp`,
  `tests/death/log_fatal.cpp` (clean-core has no fatal log level, and its own tests
  cover its logger)
- Modify: `src/core/arena.inl:30` (`VESPERA_ASSERT` → `CC_ASSERT`)
- Modify: `tests/core/test_arena.cpp`, `tests/core/test_shaped_core.cpp`
  (`VESPERA_CHECK` → `CC_ASSERT_ALWAYS`)
- Modify: `tests/death/assert_fires.cpp`
- Modify: `cmake/death_test.cmake` (message regex, only if needed)
- Modify or delete: `src/core/platform*.{h,cpp}`. Delete each function that has no
  caller left; keep the platform macros in `platform.h`.

**Interfaces:**
- Consumes: `<clean-core/common/assert.hh>`, which provides:
  - `CC_ASSERT(cond, msg)`: a debug check, replacing `VESPERA_ASSERT`
  - `CC_ASSERT_ALWAYS(cond, msg)`: always on, replacing `VESPERA_CHECK`
  - `CC_UNREACHABLE(msg)`: replaces `VESPERA_UNREACHABLE`

  Also `<clean-core/common/log.hh>`, which provides `CC_LOG_TRACE` through
  `CC_LOG_ERROR`.
- Produces: no `VESPERA_ASSERT`, `VESPERA_CHECK` or `VESPERA_LOG_*` anywhere in the
  repo.

- [ ] **Step 1: Read the replacement first**

Read `extern/shaped-core/libs/base/clean-core/src/clean-core/common/assert.hh`
(lines 1 to 70) and `record/log.hh` (lines 120 to 160).

Note two things in `docs/blog/notes/m0.md`:
- when `CC_ASSERT` is compiled out
- what text a failed assert prints

- [ ] **Step 2: Make the death test use clean-core**

`tests/death/assert_fires.cpp`:
```cpp
#include <clean-core/common/assert.hh>

int main() {
    CC_ASSERT_ALWAYS(false, "This check should fire");
}
```

- [ ] **Step 3: Run it**

Run: `cmake --build --preset win-clang && ctest --preset win-clang -R death_assert_fires`
Expected: either PASS, or FAIL with `terminated (...) without an assert/check
message`. A failure means clean-core's wording differs from the regex.

- [ ] **Step 4: Update the death test regex if needed**

If Step 3 failed, update the regex in `cmake/death_test.cmake` to match the text
from Step 1, and leave the `result STREQUAL "0"` guard alone. Temporarily change the
test to `CC_ASSERT_ALWAYS(true, "...")` and run it.
Expected: FAIL with `exited cleanly`. Revert the temporary change, run again, and
expect PASS.

- [ ] **Step 4b: Keep the Windows death tests from hanging**

clean-core's assert also ends in `std::abort()`. In a Debug build on Windows,
when output isn't a console, that shows a modal dialog and CI hangs (found
2026-11-16).
- Add `void vespera::core::init()` in `src/core/init.{h,cpp}`. On Windows it calls
  `_set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT)`; on Linux it does
  nothing.
- Call it first in `tests/death/assert_fires.cpp`, and later in the game's `main`.

Run: `ctest --preset win-clang -R death`
Expected: PASS in well under a second. It took 2.7 s while the dialog path was
active.

- [ ] **Step 5: Swap the remaining call sites and delete the old files**

Make the replacements listed under Files, then delete the old files.

Run: `git grep -n "VESPERA_ASSERT\|VESPERA_CHECK\|VESPERA_LOG\|core/assert.h\|core/logger.h"`
Expected: no output.

- [ ] **Step 6: Run all tests on every preset**

Run: `ctest --preset win-clang`, plus the Linux presets as in Task 1, Step 6.
Expected: everything passes.

- [ ] **Step 7: Commit**

```bash
git add -A src tests cmake
git commit -m "core: use clean-core's assert and logger; keep the arena"
```

---

### Task 3: Separate the engine from the game

**Files:**
- Move: `app/main.cpp` → `game/vespera/main.cpp`
- Modify: `CMakeLists.txt`:
  - rename target `vespera` → `vespera_engine`
  - rename `vespera_app` → `vespera`, with sources globbed from `game/vespera/`
  - every test links `vespera_engine`
- Create: `README.md` (build prerequisites; short)

**Interfaces:**
- Produces: library target `vespera_engine` (sources in `src/`) and executable
  `vespera`. Later milestones add to these by name.

- [ ] **Step 1: Write the failing check.** Add a test that runs the game and
  expects it to exit 0:
  ```
  add_test(NAME game_runs COMMAND vespera)
  ```
  Run: `ctest --preset win-clang -R game_runs`
  Expected: FAIL. Before the rename the target `vespera` is the library, so
  configure errors out.

- [ ] **Step 2: Rename and move**

Write `game/vespera/main.cpp` as `int main()`. It logs
`CC_LOG_INFO("Vespera {}", "0.1.0")` and returns 0.

Better: pass `PROJECT_VERSION` in with
`target_compile_definitions(vespera PRIVATE VESPERA_VERSION="${PROJECT_VERSION}")`
and log that, so the version has a single source.

- [ ] **Step 3: Run**

Run: `cmake --preset win-clang && cmake --build --preset win-clang && ctest --preset win-clang`
Expected: everything passes, including `game_runs`, and the log shows
`Vespera 0.1.0`.

- [ ] **Step 4: Write `README.md`.** Keep it to six lines or fewer:
  - clone with `--recursive`
  - Linux: Clang 19 or newer, selected through `CMakeUserPresets.json` (see Global
    Constraints)
  - on Windows, configure from a "Developer PowerShell for VS 2022", the reason
    being shaped-core's ConfigureEnvironment
  - the presets
  - `ctest --preset <name>`

- [ ] **Step 5: Commit**

```bash
git add -A app game CMakeLists.txt README.md
git commit -m "build: split vespera_engine library from the vespera game"
```

---

### Task 4: Blog post 0

**Files:**
- Create: `docs/blog/notes/m0.md` (gathered during Tasks 1 to 3)
- Create: `docs/blog/000-why-build-an-engine.md`

The assistant reviews the post for accuracy. The owner writes the prose.

- [ ] **Step 1: Outline the post.** A suggested outline:
  1. What Vespera is: the game in three sentences, plus the look, the sound, and
     the reason for VR.
  2. Why build an engine solo: awareness of every component. "Building the
     engine *for* the game" is the rule that keeps it manageable.
  3. The stack and why it's small:
     - shaped-core and SGL
     - Vulkan
     - OpenXR, through shaped-xr
     - the OS APIs for the window, input and audio
     - nothing else, and why
  4. The plan: the vertical slice reached bottom-up (M0 to M5), then deepening
     (M6 to M11).
  5. What M0 did, with one honest snag from `notes/m0.md`.
- [ ] **Step 2: Draft the post.**
- [ ] **Step 3: Fact-check.** The assistant checks every technical claim in the
  draft against the spec and the code. Fix whatever it finds.
- [ ] **Step 4: Commit**

```bash
git add docs/blog
git commit -m "blog: post 0, why build an engine"
```
