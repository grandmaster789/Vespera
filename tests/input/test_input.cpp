#include <input/input.h>
#include <clean-core/common/assert.hh>
#include <cmath>

#include "platform/windows/input_keyboard_windows.h"

/*static bool pretty_close(float a, float b) {
    return std::abs(a - b) < 1e-4f;
}*/

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

void test_gamepad_unplug() {
    using vespera::input::InputState;
    using vespera::input::e_GamepadButton;

    // gamepad unplug releases held buttons and zeroes axes
    InputState is;
    is.m_Gamepad.connect();
    is.m_Gamepad[e_GamepadButton::a].press();
    is.m_Gamepad.m_LeftStick = tg::vec2f { 0.5f, 0.5f };
    is.m_Gamepad.disconnect();

    CC_ASSERT_ALWAYS(!is.m_Gamepad.m_Connected,                   "disconnect() should disconnect");
    CC_ASSERT_ALWAYS(is.m_Gamepad[e_GamepadButton::a].m_Released, "the button A is released on unplug");
    CC_ASSERT_ALWAYS(is.m_Gamepad.m_LeftStick == tg::vec2f::zero, "stick zeroed");
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

}

#else
    #error "Unsupported platform"
#endif

int main() {
    test_button_taps();
    test_focus_loss();
    test_gamepad_unplug();

#if VESPERA_PLATFORM == VESPERA_PLATFORM_WINDOWS
    test_win32_scancode();
#elif VESPERA_PLATFORM == VESPERA_PLATFORM_LINUX
    test_linux_evdev_code();
#else
    #error "Unsupported platform"
#endif
}