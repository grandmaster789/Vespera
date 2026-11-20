# M1 Window and Input Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.
>
> **For this project:** the owner implements the tasks (see the spec's
> "Working model"). The assistant explains each task before it starts and reviews
> it after. CMake, presets and CI steps are the assistant's.

**Goal:**
- The game opens a native window on Windows (Win32) and Linux (xcb).
- Keyboard, mouse and one gamepad (XInput on Windows, evdev on Linux) update one
  input state.
- A fixed-step main loop drives the game. Closing the window or pressing Escape
  exits cleanly.

**Architecture:**
- `src/platform/` holds one header per concept, with two implementations behind
  it: `src/platform/windows/` and `src/platform/linux/`. CMake compiles only the
  current OS's directory, so the platform files need no `#if` guards.
- Window and gamepad hide their OS state behind a private `Impl`, held in a
  `std::unique_ptr`. That keeps the `Impl` at a stable address: the Win32 window
  procedure keeps a pointer to it, so it must not move. It also keeps OS headers
  out of the public headers.
- Everything that can be pure is pure and unit-tested on both OSes: the
  fixed-step accumulator, the button edge latches, key-code translation and
  gamepad normalisation. The OS code only feeds events into them.
- The main loop lives in the game's `main`. The engine provides the pieces.

**Tech Stack:**
- C++23, CMake 3.28, Ninja; presets `win-clang`, `linux-clang`, `linux-clang-asan`.
- shaped-core `c6c96c7`: `clean-core`, `typed-geometry`. No sg yet (M2).
- Win32 (`user32`, `xinput`), xcb (`libxcb`), Linux evdev (`<linux/input.h>`).

**Spec:** `docs/superpowers/specs/2026-11-16-vespera-engine-roadmap-design.md`

## Global Constraints

- Warnings are errors on every Vespera target (`vespera_target_defaults`).
- shaped-core is pinned to `c6c96c7` and never modified in place.
- New dependencies in M1: OS APIs only. That means `user32` and `xinput` on
  Windows, and `libxcb` on Linux. No SDL, no xkbcommon, no libudev.
- Windows and Linux are equal. Every task passes CI:
  - `win-clang` on `windows-2025`, which has VS 2022. `windows-latest` has VS
    2026, whose STL rejects Clang older than 22.
  - `linux-clang-asan` on `ubuntu-26.04`.
  - `linux-clang` natively on `ubuntu-26.04-arm`.
- Clang 19 or newer (CI: clang-cl 20 on Windows, Clang 21 on Linux).
- Engine code (`src/`) never refers to the game (`game/`).
- Naming follows the existing code:
  - namespaces `vespera::core` and `vespera::platform`
  - `PascalCase` classes, `snake_case` functions, `e_` prefix on enums
  - `.h` headers with `VESPERA_<DIR>_<NAME>_H` include guards
  - tests assert with `CC_ASSERT_ALWAYS`

**Scope cuts (one line each):**
- **Action layer:** M3. M1 exposes raw keys, mouse and gamepad.
- **Text input and keyboard layouts:** none. Keys are **physical positions** (WASD
  is WASD on AZERTY too). Text entry arrives with ImGui in M2.
- **Raw Input:** not in M1. Mouse position comes from `WM_MOUSEMOVE`/`MotionNotify`
  and the delta is the difference between positions. If M3's mouse aiming needs
  unaccelerated deltas, Raw Input (and XInput2 on Linux) arrives then.
- **Gamepads:** one, the first connected. No hotplug notifications:
  - XInput: empty slots are probed at most once a second, because
    `XInputGetState` on an empty slot is slow.
  - evdev: `/dev/input/event*` is rescanned at most once a second while no pad is
    open.
  - A device that can't be opened (`EACCES`) is logged once, and the game runs
    without it.
- **Vibration, multiple pads, Wayland-native windows:** none. xcb runs on Wayland
  through XWayland.

## Review Focus

1. **Focus loss while a key is held.** You alt-tab while holding W. Expected: W
   reads as released, not stuck down. The window calls `InputState::release_all()`
   on `WM_KILLFOCUS` and `FocusOut`. Tested in Task 2 (`release_all`), and checked by
   hand in Task 3.
2. **X11 key auto-repeat.** Holding a key on X11 sends release/press pairs. A
   naive implementation fires `pressed` on every repeat. Expected: one `pressed`
   per physical press. `ButtonState::press()` ignores presses while down, which
   covers Windows repeats. The xcb backend also has to drop a `KeyRelease` that is
   immediately followed by a `KeyPress` of the same key with the same timestamp.
   Checked by hand in Task 3.
3. **A long stall** from a Windows drag or resize (the modal loop blocks `main`),
   a debugger break, or a slow first frame. Expected: no burst of hundreds of
   catch-up steps. `FixedStep::advance` runs at most `max_steps_per_advance`
   steps and drops the rest. Tested in Task 1.
4. **Closing the window.** On xcb the close button only sends a `WM_PROTOCOLS`
   client message, and only if the window has registered `WM_DELETE_WINDOW`.
   Without that, the window manager kills the connection and the next xcb call
   fails. Expected on both OSes: `close_requested()` becomes true and the game
   exits with code 0. Tested in Task 3: the window test posts a close to itself.
5. **Gamepad unplugged mid-run.** Expected: `connected` turns false, every held
   button reads `released` once, sticks and triggers read zero, and the pad is
   picked up again when it is plugged back in. `GamepadState::disconnect()` is
   tested in Task 2; the unplug itself is checked by hand in Task 4.

---

### Task 1: Fixed-step clock

**Files:**
- Create: `src/core/fixed_step.h`, `src/core/fixed_step.cpp`
- Test: `tests/core/test_fixed_step.cpp`

**Interfaces:**
- Produces:
  ```cpp
  namespace vespera::core {
      class FixedStep {
      public:
          explicit FixedStep(double step_secs, int max_steps_per_advance = 8);
          // Banks elapsed time and returns how many steps are due now, at most
          // max_steps_per_advance. Time beyond that cap is dropped.
          [[nodiscard]] int advance(double elapsed_secs);
          [[nodiscard]] double alpha() const; // banked time / step_secs, in [0, 1)
      };
  }
  ```
  `CC_ASSERT` that `step_secs > 0`, `max_steps_per_advance > 0` and
  `elapsed_secs >= 0`. A steady clock never runs backwards, so a negative value is a
  bug.

- [ ] **Step 1: Write the failing test** `tests/core/test_fixed_step.cpp`:

```cpp
#include <core/fixed_step.h>
#include "clean-core/common/assert.hh"
#include <cmath>

using vespera::core::FixedStep;

static bool near(double a, double b) { return std::abs(a - b) < 1e-9; }

int main() {
    FixedStep step(0.01);
    CC_ASSERT_ALWAYS(step.advance(0.0) == 0, "no time, no steps");
    CC_ASSERT_ALWAYS(step.advance(0.025) == 2, "2.5 steps of time runs 2");
    CC_ASSERT_ALWAYS(near(step.alpha(), 0.5), "half a step stays banked");
    CC_ASSERT_ALWAYS(step.advance(0.005) == 1, "banked half plus a half is one step");
    CC_ASSERT_ALWAYS(near(step.alpha(), 0.0), "nothing left over");

    // A 10 s stall (debugger, window drag) runs at most 8 steps, not 1000.
    CC_ASSERT_ALWAYS(step.advance(10.0) == 8, "a stall is capped");
    CC_ASSERT_ALWAYS(step.alpha() < 1.0, "the excess is dropped, not banked");
    CC_ASSERT_ALWAYS(step.advance(0.0) == 0, "and does not come back later");

    FixedStep capped(0.01, 3);
    CC_ASSERT_ALWAYS(capped.advance(1.0) == 3, "the cap is configurable");
}
```

- [ ] **Step 2: Build. It fails** because `core/fixed_step.h` doesn't exist.

Run: `cmake --build --preset win-clang`
Expected: FAIL, `'core/fixed_step.h' file not found`.

- [ ] **Step 3: Implement `FixedStep`** in `src/core/fixed_step.{h,cpp}`. A `double`
accumulator. When the cap is hit, empty the bank. Keeping the remainder instead
would save less than one step of time, which nobody sees after a stall.

- [ ] **Step 4: Run the tests. They pass.**

Run: `ctest --preset win-clang`
Expected: `core_test_fixed_step` passes, along with the 4 existing tests.

- [ ] **Step 5: Commit** `core: fixed-step accumulator`.

---

### Task 2: Input state, key codes and gamepad math (pure, both OSes)

**Files:**
- Create: `src/platform/input.h`, `src/platform/input.cpp`
- Test: `tests/platform/test_input.cpp` (becomes `platform_test_input`)

**Interfaces:**
- Produces (all in `namespace vespera::platform`):
  ```cpp
  enum class e_Key : std::uint8_t {
      a, b, /* ... */ z, digit_0, /* ... */ digit_9,
      space, enter, escape, tab, backspace,
      left_shift, right_shift, left_ctrl, right_ctrl, left_alt, right_alt,
      up, down, left, right, f1, /* ... */ f12, count };
  enum class e_MouseButton : std::uint8_t { left, right, middle, count };
  enum class e_GamepadButton : std::uint8_t {
      a, b, x, y, left_shoulder, right_shoulder, back, start,
      left_thumb, right_thumb, dpad_up, dpad_down, dpad_left, dpad_right, count };

  struct ButtonState {
      bool down = false;
      bool pressed = false;  // went down since the last end_step
      bool released = false; // went up since the last end_step
      void press();          // ignored while already down (key repeat)
      void release();        // ignored while already up
      void clear_edges();
  };

  struct MouseState {
      tg::pos2i position;    // client pixels, origin top-left
      tg::vec2i delta;       // summed since the last end_step
      float wheel = 0;       // notches, summed since the last end_step
      std::array<ButtonState, size_t(e_MouseButton::count)> buttons;
  };

  struct GamepadState {
      bool connected = false;
      tg::vec2f left_stick, right_stick; // [-1, 1], +y is up, deadzone applied
      float left_trigger = 0, right_trigger = 0; // [0, 1]
      std::array<ButtonState, size_t(e_GamepadButton::count)> buttons;
      void disconnect(); // releases held buttons, zeroes axes, connected = false
  };

  struct InputState {
      std::array<ButtonState, size_t(e_Key::count)> keys;
      MouseState mouse;
      GamepadState gamepad;
      [[nodiscard]] ButtonState const& key(e_Key k) const;
      void end_step();    // clears every edge, mouse delta and wheel; after each sim step
      void release_all(); // releases keys and mouse buttons; on focus loss
  };

  // Physical key translation. On Windows: scan code from WM_KEYDOWN's lParam
  // (bits 16-23) plus the extended bit (24). On Linux: the evdev code, which is the
  // X keycode minus 8. Keys outside e_Key give nullopt.
  std::optional<e_Key> key_from_win32_scancode(std::uint16_t scancode, bool extended);
  std::optional<e_Key> key_from_evdev_code(std::uint16_t code);

  // Gamepad normalisation, shared by XInput and evdev.
  float normalize_axis(std::int32_t value, std::int32_t min, std::int32_t max);    // min -> -1, max -> +1
  float normalize_trigger(std::int32_t value, std::int32_t min, std::int32_t max); // min -> 0, max -> 1
  tg::vec2f apply_radial_deadzone(tg::vec2f stick, float deadzone);
  // Below the deadzone the result is 0. Above it, the length is rescaled so it starts
  // at 0 on the deadzone's edge and reaches 1 at full tilt, clamped to length 1.
  inline constexpr float k_left_stick_deadzone  = 7849.0f / 32767.0f; // XInput's documented values,
  inline constexpr float k_right_stick_deadzone = 8689.0f / 32767.0f; // used on Linux too
  ```

  A pressed edge is a latch, not a comparison between two frames, for two
  reasons:
  - A tap that goes down and up inside one frame still reads `pressed` for one
    step.
  - With several steps in one frame, only the first step sees it.

- [ ] **Step 1: Write the failing test** `tests/platform/test_input.cpp`:

```cpp
#include <platform/input.h>
#include "clean-core/common/assert.hh"
#include <cmath>

using namespace vespera::platform;

static bool near(float a, float b) { return std::abs(a - b) < 1e-4f; }

int main() {
    // Edges: a tap inside one frame is seen once.
    ButtonState b;
    b.press(); b.release();
    CC_ASSERT_ALWAYS(!b.down && b.pressed && b.released, "a tap keeps both edges");
    b.clear_edges();
    CC_ASSERT_ALWAYS(!b.pressed && !b.released, "edges last one step");

    // Key repeat: a second press while down is not a new press.
    b.press(); b.clear_edges(); b.press();
    CC_ASSERT_ALWAYS(b.down && !b.pressed, "repeat is not a press");

    // Focus loss releases held keys.
    InputState input;
    input.keys[size_t(e_Key::w)].press();
    input.end_step();
    input.release_all();
    CC_ASSERT_ALWAYS(!input.key(e_Key::w).down && input.key(e_Key::w).released, "focus loss releases W");

    // end_step consumes mouse motion.
    input.mouse.delta = {3, 4}; input.mouse.wheel = 1;
    input.end_step();
    CC_ASSERT_ALWAYS(input.mouse.delta == tg::vec2i(0, 0) && input.mouse.wheel == 0, "delta is per step");

    // Unplug releases held buttons and zeroes axes.
    input.gamepad.connected = true;
    input.gamepad.buttons[size_t(e_GamepadButton::a)].press();
    input.gamepad.left_stick = {0.5f, 0.5f};
    input.gamepad.disconnect();
    CC_ASSERT_ALWAYS(!input.gamepad.connected, "disconnected");
    CC_ASSERT_ALWAYS(input.gamepad.buttons[size_t(e_GamepadButton::a)].released, "A released on unplug");
    CC_ASSERT_ALWAYS(input.gamepad.left_stick == tg::vec2f(0, 0), "stick zeroed");

    // Key codes. Set-1 scan codes and evdev codes agree outside the extended keys.
    CC_ASSERT_ALWAYS(key_from_win32_scancode(0x11, false) == e_Key::w, "win32 W");
    CC_ASSERT_ALWAYS(key_from_win32_scancode(0x48, true) == e_Key::up, "win32 up is E0 48");
    CC_ASSERT_ALWAYS(key_from_win32_scancode(0x1D, true) == e_Key::right_ctrl, "win32 right ctrl is E0 1D");
    CC_ASSERT_ALWAYS(key_from_win32_scancode(0x1D, false) == e_Key::left_ctrl, "win32 left ctrl");
    CC_ASSERT_ALWAYS(key_from_evdev_code(17) == e_Key::w, "evdev KEY_W");
    CC_ASSERT_ALWAYS(key_from_evdev_code(103) == e_Key::up, "evdev KEY_UP");
    CC_ASSERT_ALWAYS(key_from_evdev_code(1) == e_Key::escape, "evdev KEY_ESC");
    CC_ASSERT_ALWAYS(!key_from_evdev_code(0).has_value(), "KEY_RESERVED is no key");

    // Gamepad math.
    CC_ASSERT_ALWAYS(near(normalize_axis(-32768, -32768, 32767), -1.0f), "axis min");
    CC_ASSERT_ALWAYS(near(normalize_axis(32767, -32768, 32767), 1.0f), "axis max");
    CC_ASSERT_ALWAYS(std::abs(normalize_axis(0, -32768, 32767)) < 1e-4f, "axis centre");
    CC_ASSERT_ALWAYS(near(normalize_trigger(1023, 0, 1023), 1.0f), "trigger max");
    CC_ASSERT_ALWAYS(apply_radial_deadzone({0.1f, 0.1f}, 0.24f) == tg::vec2f(0, 0), "inside the deadzone");
    CC_ASSERT_ALWAYS(near(apply_radial_deadzone({1.0f, 0.0f}, 0.24f)[0], 1.0f), "full tilt stays 1");
    CC_ASSERT_ALWAYS(near(apply_radial_deadzone({0.62f, 0.0f}, 0.24f)[0], 0.5f), "rescaled from the edge");
    auto diag = apply_radial_deadzone({1.0f, 1.0f}, 0.24f);
    CC_ASSERT_ALWAYS(near(diag[0] * diag[0] + diag[1] * diag[1], 1.0f), "corners clamp to length 1");
}
```

The scan and evdev codes come from the documented tables:
- Set-1 scan codes: W = `0x11`, Up = `E0 48`, Right Ctrl = `E0 1D`.
- `linux/input-event-codes.h`: `KEY_W` = 17, `KEY_UP` = 103, `KEY_ESC` = 1.
- Write the rest of both tables from the same two sources.

- [ ] **Step 2: Build. It fails**, because `platform/input.h` doesn't exist.

- [ ] **Step 3: Implement** `src/platform/input.{h,cpp}`. The translations are
`switch` statements. `normalize_axis` maps linearly and clamps to [-1, 1].

- [ ] **Step 4: Run the tests. They pass** on `win-clang` (and on Linux in CI).

- [ ] **Step 5: Commit** `platform: input state, key codes, gamepad math`.

---

### Task 3: The window (Win32 and xcb), keyboard and mouse

**Files:**
- Create: `src/platform/window.h`
- Create: `src/platform/windows/window_windows.cpp`
- Create: `src/platform/linux/window_xcb.cpp`
- Modify: `src/platform/windows/platform_windows.h`. It defines `NOUSER`, `NOMSG`,
  `NOWINMESSAGES`, `NOWINSTYLES`, `NOSHOWWINDOW` and `NOVIRTUALKEYCODES`, which
  hide `CreateWindowExW`, `MSG`, `WM_*`, `WS_*` and `ShowWindow`. Drop the defines
  the window code needs.
- Modify (assistant): `CMakeLists.txt`, `.github/workflows/ci.yml`, `README.md`
- Test: `tests/platform/test_window.cpp`

**Interfaces:**
- Consumes: `InputState`, `ButtonState`, `key_from_win32_scancode`,
  `key_from_evdev_code` (Task 2).
- Produces:
  ```cpp
  namespace vespera::platform {
      struct WindowDesc {
          std::string_view title = "Vespera";
          int width = 1280; // client area, physical pixels
          int height = 720;
      };

      // What M2 needs to fill sg::native_window, without including sg or OS headers.
      struct NativeWindow {
          void* hwnd = nullptr;           // Windows
          void* xcb_connection = nullptr; // Linux: xcb_connection_t*
          std::uint32_t xcb_window = 0;   // Linux: xcb_window_t
      };

      class Window {
      public:
          // Fails with a readable message, e.g. no X display (DISPLAY unset).
          static std::expected<Window, std::string> create(WindowDesc const& desc);
          ~Window();
          Window(Window&&) noexcept;
          Window& operator=(Window&&) noexcept;
          Window(Window const&) = delete;
          Window& operator=(Window const&) = delete;

          // Drains the OS queue without blocking: keys, mouse, focus, size, close.
          void poll_events(InputState& input);
          [[nodiscard]] bool close_requested() const;
          [[nodiscard]] tg::vec2i client_size() const; // 0x0 while minimised
          [[nodiscard]] NativeWindow native() const;

      private:
          struct Impl;
          explicit Window(std::unique_ptr<Impl> impl);
          std::unique_ptr<Impl> m_impl;
      };
  }
  ```

**OS facts each backend must handle:**
- **Win32:**
  - Call `SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2)`
    before creating the window. Otherwise Windows scales the window and M2's
    swapchain renders blurry.
  - Size the window with `AdjustWindowRectExForDpi`, so the *client* area is
    `width` x `height`.
  - Store the `Impl*` with `SetWindowLongPtrW(GWLP_USERDATA)`, from the
    `CREATESTRUCTW` you get in `WM_NCCREATE`.
  - Handle `WM_SYSKEYDOWN` and `WM_SYSKEYUP` as well as the plain key messages, or
    Alt and F10 never arrive.
  - `WM_CLOSE` sets the close flag. Don't call `DestroyWindow` there; the destructor
    does.
  - On `WM_KILLFOCUS`, call `release_all()`.
- **xcb:**
  - `xcb_connect(nullptr, ...)`, then check `xcb_connection_has_error`. Fail with
    "no X display" when it's set.
  - Event mask: key press/release, button press/release, pointer motion, structure
    notify, focus change.
  - Intern `WM_PROTOCOLS` and `WM_DELETE_WINDOW`, and set them on the window (Review
    Focus 4).
  - Set the title as both `WM_NAME` and `_NET_WM_NAME`.
  - Buttons 4 and 5 are the mouse wheel.
  - On `FocusOut`, call `release_all()`.
  - Drop a `KeyRelease` immediately followed by a `KeyPress` with the same keycode
    and time: that pair is auto-repeat (Review Focus 2). Use
    `xcb_poll_for_queued_event` to look at the next event.

- [ ] **Step 1 (assistant): CMake and CI**
  - In `CMakeLists.txt`, filter the engine glob: drop `/src/platform/linux/` on
    Windows and `/src/platform/windows/` elsewhere.
  - On Linux, `find_package(PkgConfig REQUIRED)` and
    `pkg_check_modules(XCB REQUIRED IMPORTED_TARGET xcb)`, then link
    `PkgConfig::XCB` `PRIVATE` to `vespera_engine`.
  - In `ci.yml`, the Linux apt line gains `pkgconf libxcb1-dev xvfb`. The Linux
    test step runs `xvfb-run -a ctest --preset ...`. shaped-core has no window
    tests on Linux, so the first CI run confirms the package names.
  - In `README.md`, the Linux line names `libxcb1-dev`.
  - `game_runs` becomes `vespera --exit-after-steps 60`, with a ctest `TIMEOUT` of
    10 s, so a loop that never exits fails instead of hanging CI.

- [ ] **Step 2: Write the failing test** `tests/platform/test_window.cpp`:

```cpp
#include <platform/window.h>
#include <platform/input.h>
#include <core/engine.h>
#include "clean-core/common/assert.hh"

using namespace vespera::platform;

int main() {
    vespera::core::Engine engine;
    auto window = Window::create({.title = "test", .width = 320, .height = 240});
    CC_ASSERT_ALWAYS(window.has_value(), "window creation");

    InputState input;
    for (int i = 0; i < 10; ++i)
        window->poll_events(input);
    CC_ASSERT_ALWAYS(!window->close_requested(), "nobody closed it");

    auto native = window->native();
    CC_ASSERT_ALWAYS(native.hwnd != nullptr || native.xcb_connection != nullptr, "has an OS handle");

    // Ask to close the way the close button does: WM_CLOSE on Win32, a
    // WM_PROTOCOLS/WM_DELETE_WINDOW client message on xcb (send it to the window
    // with xcb_send_event, then flush). The OS calls go behind
    // #if VESPERA_PLATFORM, the test's only OS-specific code.
    post_close_like_the_os_does(native); // a static helper in this test file
    for (int i = 0; i < 100 && !window->close_requested(); ++i)
        window->poll_events(input);
    CC_ASSERT_ALWAYS(window->close_requested(), "the close message is honoured");
}
```

- [ ] **Step 3: Build. It fails**, because `platform/window.h` doesn't exist.

- [ ] **Step 4: Implement `window.h`, then the backend for the OS you're on.**
  Write the other OS's backend in its own commit, and let CI build it if you can't
  build it locally.

- [ ] **Step 5: Run the tests. They pass.**

Run: `ctest --preset win-clang`
Expected: `platform_test_window` passes. In CI, on the Linux lanes under
`xvfb-run`, it passes as well.

- [ ] **Step 6: Check by hand**, on each OS you can reach. Use a temporary debug
log of key and button `pressed` edges in `main`; Task 5 keeps a version of it.
  - A 1280x720 window titled "Vespera" opens. On Windows at 150 % scaling, the
    client area is still 1280x720 physical pixels.
  - W, the arrow keys, Right Ctrl and Alt each log once per press. Holding W logs
    once, not a stream (Review Focus 2).
  - Hold W, alt-tab away, then let go of W. W logs a release, and doesn't read as
    held when you come back (Review Focus 1).
  - Drag the window around for 5 s. The game doesn't burst-step afterwards.
  - Minimise and restore. Nothing asserts, and `client_size()` reads 0x0 while
    minimised.
  - The close button exits with code 0.

- [ ] **Step 7: Commit**, one commit per backend:
`platform: Win32 window` and `platform: xcb window`.

---

### Task 4: One gamepad (XInput and evdev)

**Files:**
- Create: `src/platform/gamepad.h`
- Create: `src/platform/windows/gamepad_xinput.cpp`
- Create: `src/platform/linux/gamepad_evdev.cpp`
- Modify (assistant): `CMakeLists.txt` links `xinput` `PRIVATE` on Windows.

**Interfaces:**
- Consumes: `GamepadState`, `normalize_axis`, `normalize_trigger`,
  `apply_radial_deadzone`, `k_*_deadzone` (Task 2).
- Produces:
  ```cpp
  namespace vespera::platform {
      class Gamepad {
      public:
          Gamepad();
          ~Gamepad();
          Gamepad(Gamepad const&) = delete;
          Gamepad& operator=(Gamepad const&) = delete;

          // Reads the first connected pad into state. While none is connected,
          // looks for one at most once a second (now_secs: steady clock).
          // Logs connect and disconnect once each.
          void poll(GamepadState& state, double now_secs);

      private:
          struct Impl;
          std::unique_ptr<Impl> m_impl;
      };
  }
  ```

**OS facts:**
- **XInput:**
  - `XInputGetState` takes user indices 0-3. Sticks are `SHORT`, with +y up.
    Triggers are `BYTE` 0-255.
  - A return of `ERROR_DEVICE_NOT_CONNECTED` on the open slot means call
    `state.disconnect()`.
  - Only probe empty slots on the once-a-second rescan.
- **evdev:**
  - Open `/dev/input/event*` with `O_RDONLY | O_NONBLOCK`. The first device whose
    `EVIOCGBIT(EV_KEY)` reports `BTN_GAMEPAD` is the pad.
  - Read each axis range with `EVIOCGABS`: `ABS_X`/`ABS_Y` (left stick),
    `ABS_RX`/`ABS_RY` (right stick), `ABS_Z`/`ABS_RZ` (triggers), and
    `ABS_HAT0X`/`ABS_HAT0Y` (the d-pad on xpad).
  - **evdev +y is down, so negate both sticks' y.**
  - A `SYN_DROPPED` event means re-read the state with `EVIOCGKEY` and `EVIOCGABS`.
  - `read` failing with `ENODEV` means the pad was unplugged: close the device and
    call `state.disconnect()`.
  - `EACCES` gets one info log naming the device, and the game runs without it.
    Desktop distributions grant the seat's user access to joysticks through
    systemd's uaccess rule.

- [ ] **Step 1: There's no automated test for the device layer.** CI has no
  gamepad, and the maths is already tested in Task 2. The checks are manual,
  in Step 3.

- [ ] **Step 2: Implement `gamepad.h` and the backend for your OS**, then the
  other one.

- [ ] **Step 3: Check by hand with a real pad**, logging stick values and button
`pressed` edges at debug level:
  - **Sticks:**
    - Pushing a stick up reads +y on both OSes.
    - A resting stick reads exactly 0.
    - Full tilt diagonally reads length 1, not 1.41.
  - **Triggers and d-pad:**
    - Each trigger goes from 0 to 1.
    - All four d-pad directions register, on Linux too.
  - **Face buttons:** A, B, X and Y log as the right names on Linux. evdev's
    `BTN_NORTH` and `BTN_WEST` are aliases of `BTN_X` and `BTN_Y`, and drivers
    disagree on which is which, so check with your pad rather than trusting the
    table.
  - **Unplug and replug:**
    - Unplug while holding A: A logs a release, and "gamepad disconnected" is logged
      once.
    - Plug it back in: it's picked up within about a second (Review Focus 5).
  - **No pad at all:** the game runs, and no error is logged.

- [ ] **Step 4: Commit** `platform: XInput gamepad` and `platform: evdev gamepad`.

---

### Task 5: The main loop

**Files:**
- Modify: `game/vespera/main.cpp`
- Modify (assistant): `CMakeLists.txt` (`game_runs` already changed in Task 3)

**Interfaces:**
- Consumes: `Engine`, `Window`, `Gamepad`, `InputState`, `FixedStep`,
  `cc::current_time_steady_secs()` and `cc::this_thread_sleep_secs(double)` from
  `clean-core/common/time.hh` and `clean-core/thread/thread.hh`, and
  `cc::scoped_scheduler_tick`.
- Produces: `vespera [--exit-after-steps N]`. It exits with 0 after N steps, when
  the window closes, or on Escape. It exits with 1, after an error log, when the
  window can't be created.

**The loop, in order:**
1. `window.poll_events(input)`, then `gamepad.poll(input.gamepad, now)`.
2. `int n = step.advance(now - last)`. For each step: `game_step(input)`, then
   `input.end_step()`.
3. Sleep until the next step is due:
   `cc::this_thread_sleep_secs(k_sim_step_secs * (1.0 - step.alpha()))`.
   Hold a `cc::scoped_scheduler_tick` for the loop's lifetime, so a 1 ms sleep on
   Windows isn't 15.6 ms. Comment it with `// ponytail:` and say that M2's present
   with vsync paces the loop instead.

- **Step rate:** a `constexpr double k_sim_step_secs = 1.0 / 120.0;` in `main.cpp`.
  This is a placeholder, not a decision: the simulation rate is independent of
  the display rate (M5 interpolates with `alpha()`), and the real choice belongs
  to M3, once there is gameplay to measure. Keep it one constant so it can move.
- **`game_step` in M1:**
  - Escape requests exit.
  - Every key, mouse button and gamepad button `pressed` edge is logged at debug
    level, with the gamepad sticks alongside. That log is how M1 is checked by
    hand.

- [ ] **Step 1: Run the smoke test. It fails or hangs** until the loop honours
  `--exit-after-steps`.

Run: `ctest --preset win-clang -R game_runs`
Expected: FAIL. The test passes an argument `main` ignores. If `main` already
loops, the 10 s timeout fails it.

- [ ] **Step 2: Implement the loop** in `game/vespera/main.cpp`.

- [ ] **Step 3: Run all the tests. They pass.**

Run: `ctest --preset win-clang`
Expected: every test passes. `game_runs` takes about 0.5 s (60 steps at 120 Hz).

- [ ] **Step 4: Check by hand:**
  - CPU use while idle is low (a few %, not one full core).
  - Escape quits with exit code 0.
  - Run the Task 3 and Task 4 manual checks once more against the real loop.

- [ ] **Step 5: Commit** `game: fixed-step main loop`, then push. CI must be green
  on all three lanes.

---

### Task 6: Notes and blog post

**Files:**
- Create: `docs/blog/notes/m1.md`. Add to it during Tasks 1-5, not after.
- Create: `docs/blog/001-window-and-input.md`

- [ ] **Step 1:** Keep notes as each task lands: snags, numbers, surprises. Likely
  material:
  - why set-1 scan codes and evdev codes match
  - why edges are latches
  - X11 auto-repeat
  - the DPI call
  - what a stall does to a fixed step
- [ ] **Step 2:** The owner drafts the post. The assistant fact-checks it, and
  adds a diagram if one helps, such as the loop's order of operations.
- [ ] **Step 3: Commit** `docs: M1 notes and blog post`.
