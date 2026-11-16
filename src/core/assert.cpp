#include "assert.h"
#include "logger.h"
#include "platform.h"

#include <cstdlib>
#include <format>

namespace {
    [[noreturn]]
    void terminate_after_failure() {
        vespera::core::break_if_debugger_attached();

#if VESPERA_PLATFORM == VESPERA_PLATFORM_WINDOWS
        // Debug CRT: abort() shows a modal Abort/Retry/Ignore box when output isn't
        // a console (CI, ctest pipes) and the process hangs. Our own log line already
        // reported the failure.
        _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
#endif
        std::abort();
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

        terminate_after_failure();
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

        terminate_after_failure();
    }
}
