#include "gamepad.h"

namespace vespera::input {
    void GamepadState::connect() {
        m_Connected = true; // placeholder
    }

    void GamepadState::disconnect() {
        m_Connected = false; // placeholder
        reset();
    }

    void GamepadState::reset() {
        // release all buttons
        for (auto& b: m_Buttons)
            b.release();

        // zero both axes
        m_LeftStick  = tg::vec2f::zero;
        m_RightStick = tg::vec2f::zero;
    }

    ButtonState& GamepadState::operator[](e_GamepadButton b) {
        return m_Buttons[static_cast<size_t>(b)];
    }

    const ButtonState& GamepadState::operator[](e_GamepadButton b) const {
        return m_Buttons[static_cast<size_t>(b)];
    }

    float GamepadState::normalize_axis(
            std::int32_t value,
            std::int32_t minimum,
            std::int32_t maximum
    ) const {
        if (value < minimum)
            return -1.0f;

        if (value > maximum)
            return 1.0f;

        // determine result in [0..1]
        float result = static_cast<float>(value) / (static_cast<float>(maximum) - static_cast<float>(minimum));

        // rescale to [-1..1]
        return (result * 2.0f) - 1.0f;
    }

    float GamepadState::normalize_trigger(
        std::int32_t value,
        std::int32_t minimum,
        std::int32_t maximum
    ) const {
        if (value < minimum)
            return 0.0f;
        if (value > maximum)
            return 1.0f;

        // directly determine the result in [0..1]
        return static_cast<float>(value) / (static_cast<float>(maximum) - static_cast<float>(minimum));
    }

    tg::vec2f GamepadState::apply_radial_deadzone(
        tg::vec2f stick,
        float     deadzone
    ) const {
        if (stick.length_sqr() < (deadzone * deadzone))
            return tg::vec2f::zero;
        else
            return stick;
    }
}