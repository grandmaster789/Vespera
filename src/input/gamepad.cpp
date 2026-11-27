#include "gamepad.h"

#include <algorithm>

namespace vespera::input {
    void GamepadState::connect() {
        m_Connected = true; // placeholder
    }

    void GamepadState::disconnect() {
        m_Connected = false; // placeholder
        reset();
    }

    void GamepadState::clear_edges() {
        for (auto& b: m_Buttons)
            b.clear_edges();
    }

    void GamepadState::reset() {
        // release all buttons, clear all edges
        for (auto& b: m_Buttons) {
            b.release();
            b.clear_edges();
        }

        // zero both axes
        m_LeftStick  = tg::vec2f::zero;
        m_RightStick = tg::vec2f::zero;

        // zero the triggers
        m_LeftTrigger  = 0.0f;
        m_RightTrigger = 0.0f;
    }

    ButtonState& GamepadState::operator[](e_GamepadButton b) {
        return m_Buttons[static_cast<std::size_t>(b)];
    }

    const ButtonState& GamepadState::operator[](e_GamepadButton b) const {
        return m_Buttons[static_cast<std::size_t>(b)];
    }

    float GamepadState::normalize_axis(
            std::int32_t value,
            std::int32_t minimum,
            std::int32_t maximum
    ) {
        CC_ASSERT_ALWAYS(minimum <= maximum, "Minimum must be lower than maximum");

        if (value <= minimum)
            return -1.0f;

        if (value >= maximum)
            return 1.0f;

        // calculate in [0..1] range
        auto result = static_cast<float>(value - minimum) / static_cast<float>(maximum - minimum);

        // rescale to [-1..1]
        return 2.0f * result - 1.0f;
    }

    float GamepadState::normalize_trigger(
        std::int32_t value,
        std::int32_t minimum,
        std::int32_t maximum
    ) {
        CC_ASSERT_ALWAYS(minimum <= maximum, "Minimum must be lower than maximum");

        if (value <= minimum)
            return 0.0f;
        if (value >= maximum)
            return 1.0f;

        // directly determine the result in [0..1]
        return static_cast<float>(value - minimum) / (static_cast<float>(maximum - minimum));
    }

    tg::vec2f GamepadState::apply_radial_deadzone(
        tg::vec2f stick,
        float     deadzone
    ) {
        auto len = stick.length();

        // inside deadzone, return zero
        // outside deadzone, rescale from the non-deadzone area
        if (len <= deadzone)
            return tg::vec2f::zero;

        // the area around the deadzone should smoothly transition from 0 to 1
        auto new_len = (std::min(len, 1.0f) - deadzone) / (1.0f - deadzone);

        return (stick / len) * new_len;
    }
}
