#include "assert.h"
#include "logger.h"
#include "platform.h"

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
