#include "platform.h"

#if VESPERA_PLATFORM == VESPERA_PLATFORM_LINUX

#include <cstdlib>
#include <cstring>
#include <cstdio>

#include "platform_linux.h"

namespace vespera::core {
    bool is_debugger_attached() {
        // Linux equivalent of testing whether a debugger is attached
        bool result = false;

        if (auto* status = std::fopen("/proc/self/status", "r")) {
            char buffer[1024] = {};

            while (std::fgets(buffer, sizeof(buffer), status)) {
                if (std::strncmp(buffer, "TracerPid:", 10) == 0) {
                    result = (std::atoi(buffer + 10) != 0);
                    break;
                }
            }

            std::fclose(status);
        }

        return result;
    }

    void break_if_debugger_attached() {
        if (is_debugger_attached())
            __builtin_debugtrap();
    }

    void debug_output(const char* message) {
        std::fprintf(stderr, "%s\n", message);
    }
}

#endif