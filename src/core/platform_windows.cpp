#include "platform.h"

#if VESPERA_PLATFORM == VESPERA_PLATFORM_WINDOWS

#include "platform_windows.h"

namespace vespera::core {
    bool is_debugger_attached() {
        return !!IsDebuggerPresent();
    }

    void break_if_debugger_attached() {
        if (is_debugger_attached())
            __debugbreak();
    }

    void debug_output(const char* message) {
        OutputDebugStringA(message);
    }
}

#endif