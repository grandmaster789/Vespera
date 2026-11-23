#ifndef VESPERA_PLATFORM_INPUT_WINDOWS_KEYBOARD_H
#define VESPERA_PLATFORM_INPUT_WINDOWS_KEYBOARD_H

#include "platform/platform.h"

#if VESPERA_PLATFORM == VESPERA_PLATFORM_WINDOWS

#include <optional>

#include "input/keyboard.h"

namespace vespera::input {
    // keys outside e_Key yield nullopt
    std::optional<e_Key> key_from_win32_scancode(std::uint16_t scancode, bool extended);
}

#endif

#endif
