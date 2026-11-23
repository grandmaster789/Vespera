#ifndef VESPERA_INPUT_GAMEPAD_H
#define VESPERA_INPUT_GAMEPAD_H

#include <cstdint>
#include <array>

#include "typed-geometry/linalg/vec.hh"

#include "button.h"

namespace vespera::input {
    enum class e_GamepadButton:
            std::uint8_t
    {
        a, b, x, y,
        left_shoulder, right_shoulder,
        back, start,
        left_thumb, right_thumb,
        dpad_up, dpad_down, dpad_left, dpad_right,
        count
    };

    struct GamepadState {
        using ButtonList = std::array<ButtonState, static_cast<size_t>(e_GamepadButton::count)>;

        void connect();
        void disconnect();
        void reset();

        [[nodiscard]]       ButtonState& operator[](e_GamepadButton button);
        [[nodiscard]] const ButtonState& operator[](e_GamepadButton button) const;

        // implementations for normalization is platform specific, but have a shared interface
        // see platform/[$]/input_gamepad_[$].cpp for the implementation
        [[nodiscard]]
        float normalize_axis(
            std::int32_t value,
            std::int32_t minimum,
            std::int32_t maximum
        ) const;

        [[nodiscard]]
        float normalize_trigger(
            std::int32_t value,
            std::int32_t minimum,
            std::int32_t maximum
        ) const;

        [[nodiscard]]
        tg::vec2f apply_radial_deadzone(
            tg::vec2f stick,
            float deadzone
        ) const;

        // XInput's documented values, also used on linux
        static constexpr float k_LeftStickDeadzone  = 7'849.0f / 32'767.0f;
        static constexpr float k_RightStickDeadzone = 8'689.0f / 32'767.0f;

        bool m_Connected = false;

        tg::vec2f m_LeftStick;        // [-1, 1] +y is up, deadzone applied
        tg::vec2f m_RightStick;       // [-1, 1] +y is up, deadzone applied
        float     m_LeftTrigger  = 0; // [0, 1]
        float     m_RightTrigger = 0; // [0, 1]

        ButtonList m_Buttons;
    };
}

#endif
