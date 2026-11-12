#include "assert.h"

#include <cstdlib>
#include <cstdio>
#include <format>

// TODO - move platform-specific includes to a central, consolidated place
//        the main reason is to ensure that the same macros are used
//        in the same order before including windows.h
#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX

    #include <windows.h>
#else

#endif

namespace {
    void break_if_debugger_attached() {
#ifdef _WIN32
        if (IsDebuggerPresent())
            __debugbreak();
#else
        // Linux equivalent of testing whether a debugger is attached
        if (auto* status = std::fopen("/proc/self/status", "r")) {
            char buffer[1024];

            while (std::fgets(buffer, sizeof(buffer), status))
                if (std::strstr(buffer, "TracerPid:") && std::atoi(buffer + 10))
                    __builtin_debugtrap();
            break;

            std::fclose(status);
        }
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
                "Assertion failed: {} at {}:{} - {}",
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
        //std::println("Check failed: {} at {}:{} - {}", condition, location.file_name(), location.line(), message);

        std::fputs(
            std::format(
                "Check failed: {} at {}:{} - {}",
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
