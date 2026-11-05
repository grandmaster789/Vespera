#ifndef VESPERA_CORE_ASSERT_H
#define VESPERA_CORE_ASSERT_H

#include <source_location>

namespace vespera::core {
    [[noreturn]] void assertion_failed(const char* condition, const char* message, const std::source_location& location);
    [[noreturn]] void check_failed(const char* condition, const char* message, const std::source_location& location);
}

#ifdef DEBUG
    #define VESPERA_ASSERT(cond, message) { if (!(cond)) { vespera::core::assertion_failed(##cond, message, std::source_location::current()); } }
#else
    #define VESPERA_ASSERT(cond, message) {}
#endif

#define VESPERA_CHECK(cond, message) { if (!(cond)) { vespera::core::check_failed(##cond, message, std::source_location::current()); } }
#define VESPERA_UNREACHABLE __builtin_unreachable();
#define VESPERA_BREAK __builtin_debugtrap();

#endif