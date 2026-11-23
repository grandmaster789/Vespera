#ifndef VESPERA_INPUT_MOUSE_H
#define VESPERA_INPUT_MOUSE_H

#include <cstdint>
#include <array>

#include <typed-geometry/linalg/vec.hh>
#include <typed-geometry/linalg/pos.hh>

#include "button.h"

namespace vespera::input {
    enum class e_MouseButton:
        std::uint8_t
    {
        left, right, middle,
        count
    };

    struct MouseState {
        using MouseButtonList = std::array<ButtonState, static_cast<std::size_t>(e_MouseButton::count)>;

        void clear_deltas();

        tg::pos2i m_Position;        // client pixels, origin is top-left
        tg::vec2i m_Delta;           // summed since last end_step
        float     m_WheelDelta = 0; // notches, summed since last end_step

        MouseButtonList m_ButtonStates;
    };
}

#endif