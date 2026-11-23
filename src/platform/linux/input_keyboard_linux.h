#ifndef VESPERA_PLATFORM_INPUT_LINUX_KEYBOARD_H
#define VESPERA_PLATFORM_INPUT_LINUX_KEYBOARD_H

#include "platform/platform.h"

#if VESPERA_PLATFORM == VESPERA_PLATFORM_LINUX

#include <optional>

#include "input/keyboard.h"

namespace vespera::input {
    // evdev code is X keycode minus 8
    // keys outside e_Key yield nullopt
    std::optional<e_Key> key_from_evdev_code(std::uint16_t code);
}

#endif

#endif
