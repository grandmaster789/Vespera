#include <input/input.h>
#include <clean-core/common/assert.hh>
#include <cmath>

#include "platform/windows/input_keyboard_windows.h"

static bool pretty_close(float a, float b) {
    return std::abs(a - b) < 1e-4f;
}

void test_button_taps() {
    using vespera::input::ButtonState;

    // A tap inside one frame is only seen once
    ButtonState bs;
    bs.press();
    bs.release();
    CC_ASSERT_ALWAYS(
        !bs.m_Down &&
         bs.m_Pressed &&
         bs.m_Released,
         "A tap keeps both edges"
    );

    bs.clear_edges();
    CC_ASSERT_ALWAYS(
        !bs.m_Pressed &&
        !bs.m_Released,
        "Edges last one step"
    );

    // Second press while down is not a new press
    bs.press();
    bs.clear_edges();
    bs.press();
    CC_ASSERT_ALWAYS(
         bs.m_Down &&
        !bs.m_Pressed,
        "Repeat is not a press"
    );
}

void test_focus_loss() {
    using vespera::input::InputState;
    using vespera::input::e_Key;

    // Focus loss releases held keys
    InputState is;

    is.m_Keyboard[e_Key::w].press();
    is.end_step();
    is.release_all();
    CC_ASSERT_ALWAYS(
        !is.m_Keyboard[e_Key::w].m_Down &&
         is.m_Keyboard[e_Key::w].m_Released,
        "Focus loss releases W"
    );

    // end_step consumes mouse motion
    is.m_Mouse.m_Delta = tg::vec2i { 3, 4 };
    is.m_Mouse.m_WheelDelta = 1.0f;
    is.end_step();
    CC_ASSERT_ALWAYS(
        is.m_Mouse.m_Delta      == tg::vec2i::zero &&
        is.m_Mouse.m_WheelDelta == 0.0f,
        "Mouse deltas are per step"
    );
}

void test_mouse_edges() {
    using vespera::input::InputState;
    using vespera::input::e_MouseButton;

    InputState is;
    is.m_Mouse[e_MouseButton::left].press();

    is.release_all();

    CC_ASSERT_ALWAYS(is.m_Mouse[e_MouseButton::left].m_Released, "release_all() should release all edges");
}

void test_gamepad_unplug() {
    using vespera::input::InputState;
    using vespera::input::e_GamepadButton;

    // gamepad unplug releases held buttons and zeroes axes
    InputState is;
    is.m_Gamepad.connect();
    is.m_Gamepad[e_GamepadButton::a].press();
    is.m_Gamepad.m_LeftStick = tg::vec2f { 0.5f, 0.5f };
    is.m_Gamepad.m_LeftTrigger = 0.9f;
    is.m_Gamepad.disconnect();

    CC_ASSERT_ALWAYS(!is.m_Gamepad.m_Connected,                   "disconnect() should disconnect");
    CC_ASSERT_ALWAYS(is.m_Gamepad[e_GamepadButton::a].m_Released, "the button A is released on unplug");
    CC_ASSERT_ALWAYS(is.m_Gamepad.m_LeftStick == tg::vec2f::zero, "stick zeroed on disconnect");
    CC_ASSERT_ALWAYS(is.m_Gamepad.m_LeftTrigger == 0.0f,          "trigger zeroed on disconnect");
}

void test_gamepad_edges() {
    using vespera::input::InputState;
    using vespera::input::e_GamepadButton;

    InputState is;
    is.m_Gamepad[e_GamepadButton::a].press();
    is.end_step();

    CC_ASSERT_ALWAYS(!is.m_Gamepad[e_GamepadButton::a].m_Pressed, "end_step should reset gamepad button presses");
}

void test_gamepad_math() {
    using vespera::input::GamepadState;

    CC_ASSERT_ALWAYS(pretty_close(GamepadState::normalize_axis(-32768, -32768, 32767), -1.0f), "axis min");
    CC_ASSERT_ALWAYS(pretty_close(GamepadState::normalize_axis(32767, -32768, 32767), 1.0f),   "axis max");
    CC_ASSERT_ALWAYS(std::abs(GamepadState::normalize_axis(0, -32768, 32767)) < 1e-4f,         "axis centre");

    CC_ASSERT_ALWAYS(pretty_close(GamepadState::normalize_trigger(1023, 0, 1023), 1.0f),                        "trigger max");
    CC_ASSERT_ALWAYS(GamepadState::apply_radial_deadzone(tg::vec2f{0.1f, 0.1f}, 0.24f) == tg::vec2f(0, 0),      "inside the deadzone");
    CC_ASSERT_ALWAYS(pretty_close(GamepadState::apply_radial_deadzone(tg::vec2f{1.0f, 0.0f}, 0.24f)[0], 1.0f),  "full tilt stays 1");
    CC_ASSERT_ALWAYS(pretty_close(GamepadState::apply_radial_deadzone(tg::vec2f{0.62f, 0.0f}, 0.24f)[0], 0.5f), "rescaled from the edge");
    auto diag = GamepadState::apply_radial_deadzone(tg::vec2f{1.0f, 1.0f}, 0.24f);

    CC_ASSERT_ALWAYS(pretty_close(diag[0] * diag[0] + diag[1] * diag[1], 1.0f), "corners clamp to length 1");
}

#if VESPERA_PLATFORM == VESPERA_PLATFORM_WINDOWS

void test_win32_scancode() {
    using vespera::input::key_from_win32_scancode;
    using vespera::input::e_Key;

    // https://learn.microsoft.com/en-us/windows/win32/inputdev/about-keyboard-input
    CC_ASSERT_ALWAYS(key_from_win32_scancode(0x11, false) == e_Key::w,             "Win32 W");
    CC_ASSERT_ALWAYS(key_from_win32_scancode(0x48, true)  == e_Key::up,            "Win32 up arrow extended-key (0xE048)");
    CC_ASSERT_ALWAYS(key_from_win32_scancode(0x1D, true)  == e_Key::right_control, "Win32 right control extended-key  (0xE01D)");
    CC_ASSERT_ALWAYS(key_from_win32_scancode(0x1D, false) == e_Key::left_control,  "Win32 left control");
}

#elif VESPERA_PLATFORM == VESPERA_PLATFORM_LINUX

void test_linux_evdev_code() {
    CC_ASSERT_ALWAYS(key_from_evdev_code(17)  == e_Key::w,      "evdev KEY_W");
    CC_ASSERT_ALWAYS(key_from_evdev_code(103) == e_Key::up,     "evdev KEY_UP");
    CC_ASSERT_ALWAYS(key_from_evdev_code(1)   == e_Key::escape, "evdev KEY_ESC");
    CC_ASSERT_ALWAYS(!key_from_evdev_code(0).has_value(),       "KEY_RESERVED is no key");
}

#else
    #error "Unsupported platform"
#endif

int main() {
    test_button_taps();
    test_focus_loss();
    test_mouse_edges();
    test_gamepad_unplug();
    test_gamepad_math();
    test_gamepad_edges();

#if VESPERA_PLATFORM == VESPERA_PLATFORM_WINDOWS
    test_win32_scancode();
#elif VESPERA_PLATFORM == VESPERA_PLATFORM_LINUX
    test_linux_evdev_code();
#else
    #error "Unsupported platform"
#endif
}