#include "assert.h"

#include <cstdlib>
#include <cstdio>
#include <format>

// TODO - move platform-specific includes to a central, consolidated place
#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX

    #include <windows.h>
#else
#endif

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

#ifdef _WIN32
    if (IsDebuggerPresent())
        __debugbreak();
#else
    // read from /proc/self/status and if TracePid is nonzero
    __builtin_debugtrap();
#endif

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

#ifdef _WIN32
        if (IsDebuggerPresent())
            __debugbreak();
#else
        // read from /proc/self/status and if TracePid is nonzero
        __builtin_debugtrap();
#endif

        std::abort();
    }
}
