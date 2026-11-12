#include "assert.h"

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <format>

// TODO - move platform-specific includes to a central, consolidated place
//        the main reason is to ensure that the same macros are used
//        in the same order before including windows.h
#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX

    #include <windows.h>
#endif

namespace {
    bool is_debugger_attached() {
#ifdef _WIN32
        return !!IsDebuggerPresent();
#else
        // Linux equivalent of testing whether a debugger is attached
        bool result = false;

        if (auto* status = std::fopen("/proc/self/status", "r")) {
            char buffer[1024] = {};

            while (std::fgets(buffer, sizeof(buffer), status)) {
                if (std::strncmp(buffer, "TracerPid:", 10) == 0) {
                    result = true;
                    break;
                }
            }

            std::fclose(status);
        }

        return result;
#endif
    }

    void break_if_debugger_attached() {
        if (is_debugger_attached())
#ifdef _WIN32
            __debugbreak();
#else
            __builtin_trap();
#endif
    }
}

namespace vespera::core {
    [[noreturn]]
    void assertion_failed(
        const char* condition,
        const char* message,
        const std::source_location& location
    ) {
        // placeholder until logging is implemented
        std::fputs(
            std::format(
                "Assertion failed: {} at {}:{} - {}\n",
                condition,
                location.file_name(),
                location.line(),
                message
            ).c_str(),
            stderr
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
        // placeholder until logging is implemented
        std::fputs(
            std::format(
                "Check failed: {} at {}:{} - {}\n",
                condition,
                location.file_name(),
                location.line(),
                message
            ).c_str(),
            stderr
        );

        break_if_debugger_attached();
        std::abort();
    }
}
