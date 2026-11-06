#include "assert.h"

#include <cstdlib>
#include <cstdio>
#include <format>

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

        VESPERA_BREAK;

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

        VESPERA_BREAK;

        std::abort();
    }
}
