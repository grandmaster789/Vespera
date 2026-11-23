#include "button.h"

namespace vespera::input {
    void ButtonState::press() {
        if (!m_Down) {
            m_Down    = true;
            m_Pressed = true;
        }
    }

    void ButtonState::release() {
        if (m_Down) {
            m_Down     = false;
            m_Released = true;
        }
    }

    void ButtonState::clear_edges() {
        m_Pressed  = false;
        m_Released = false;
    }
}