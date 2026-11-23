#include "keyboard.h"

namespace vespera::input {
    void KeyboardState::clear_all_edges() {
        for (auto& button: m_Keys)
            button.clear_edges();
    }

    void KeyboardState::release_all() {
        for (auto& button: m_Keys)
            button.release();
    }

    ButtonState& KeyboardState::operator[](e_Key k) {
        return m_Keys[static_cast<std::size_t>(k)];
    }

    const ButtonState& KeyboardState::operator[](e_Key k) const {
        return m_Keys[static_cast<std::size_t>(k)];
    }
}
