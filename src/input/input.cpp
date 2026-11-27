#include "input.h"

namespace vespera::input {
    void InputState::end_step() {
        // clear every edge, mouse delta and wheel after each simulation step
        m_Keyboard.clear_all_edges();
        m_Mouse.clear_deltas();
        m_Mouse.clear_edges();
        m_Gamepad.clear_edges();
    }

    void InputState::release_all() {
        // releases keys and mouse buttons, for example on focus loss
        m_Keyboard.release_all();
        m_Mouse.release_all();
    }
}