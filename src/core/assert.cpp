#include "assert.h"
#include "logger.h"
#include "platform.h"

#include <cstdio>

#if VESPERA_PLATFORM != VESPERA_PLATFORM_WINDOWS
    #include <cstdlib>
    #include <cstring>
#endif

namespace {
    bool is_debugger_attached() {
#if VESPERA_PLATFORM == VESPERA_PLATFORM_WINDOWS
            return !!IsDebuggerPresent();
#else
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
#endif
    }

    void break_if_debugger_attached() {
        if (is_debugger_attached()) {
            if constexpr (vespera::core::is_windows) {
                __debugbreak();
            }
            else {
                __builtin_debugtrap();
            }
        }
    }
}

namespace vespera::core {
    [[noreturn]]
    void assertion_failed(
        const char* condition,
        const char* message,
        const std::source_location& location
    ) {
        log_message({
                .m_Location = location,
                .m_Level    = e_LogLevel::err,
            },
            std::format("Assertion failed: {} => {}", condition, message)
        );

        break_if_debugger_attached();
        std::abort();
    }

    [[noreturn]]
    void check_failed(
        const char* condition,
        const char* message,
        const std::source_location& location
    ) {
        log_message({
                .m_Location = location,
                .m_Level    = e_LogLevel::err,
            },
            std::format("Check failed: {} => {}", condition, message)
        );

        break_if_debugger_attached();
        std::abort();
    }
}
