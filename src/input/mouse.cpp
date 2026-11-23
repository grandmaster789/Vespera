#include "mouse.h"

namespace vespera::input {
    void MouseState::clear_deltas() {
        m_Delta      = tg::vec2i::zero;
        m_WheelDelta = 0.0f;
    }

    void MouseState::clear_edges() {
        for (auto& b: m_ButtonStates)
            b.clear_edges();
    }

    void MouseState::release_all() {
        for (auto& b: m_ButtonStates)
            b.release();
    }

    void MouseState::reset() {
        clear_deltas();
        clear_edges();
    }
}
