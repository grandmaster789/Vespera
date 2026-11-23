#include "mouse.h"

namespace vespera::input {
    void MouseState::clear_deltas() {
        m_Delta      = tg::vec2i::zero;
        m_WheelDelta = 0.0f;
    }
}
