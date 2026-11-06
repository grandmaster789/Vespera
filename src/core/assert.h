#ifndef VESPERA_CORE_ASSERT_H
#define VESPERA_CORE_ASSERT_H

#include <source_location>
#include <utility>

namespace vespera::core {
    [[noreturn]] void assertion_failed(const char* condition, const char* message, const std::source_location& location);
    [[noreturn]] void check_failed(const char* condition, const char* message, const std::source_location& location);
}

#ifdef NDEBUG
    #define VESPERA_ASSERT(cond, message) do { (void)sizeof(cond); } while(0)
#else
    #define VESPERA_ASSERT(cond, message) do { if (!(cond)) { vespera::core::assertion_failed(#cond, message, std::source_location::current()); } } while (0)
#endif

#define VESPERA_CHECK(cond, message) do { if (!(cond)) { vespera::core::check_failed(#cond, message, std::source_location::current()); } } while(0)
#define VESPERA_UNREACHABLE std::unreachable()
#define VESPERA_BREAK __builtin_debugtrap()

#endif