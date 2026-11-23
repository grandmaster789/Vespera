#ifndef VESPERA_INPUT_INPUT_H
#define VESPERA_INPUT_INPUT_H

#include "gamepad.h"
#include "keyboard.h"
#include "mouse.h"

namespace vespera::input {
    struct InputState {
        void end_step();    // clear every edge, mouse delta and wheel after each simulation step
        void release_all(); // releases keys and mouse buttons, for example on focus loss

        KeyboardState m_Keyboard;
        MouseState    m_Mouse;
        GamepadState  m_Gamepad;
    };
}

#endif