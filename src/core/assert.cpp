#include "assert.h"

#include <cstdlib>
#include <print>

namespace vespera::core {
    [[noreturn]]
    void assertion_failed(
        const char* condition,
        const char* message,
        const std::source_location& location
    ) {
        // placeholder until logging is implemented
        std::println("Assertion failed: {} at {}:{} - {}", condition, location.file_name(), location.line(), message);
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
        std::println("Check failed: {} at {}:{} - {}", condition, location.file_name(), location.line(), message);
        VESPERA_BREAK;
        std::abort();
    }
}
