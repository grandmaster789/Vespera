#ifndef VESPERA_CORE_LOGGER_LOG_LEVEL_H
#define VESPERA_CORE_LOGGER_LOG_LEVEL_H

#include <format>

namespace vespera::core::logger {
    enum class e_LogLevel {
        fatal,
        err,
        warning,
        info,
        debug,
        trace
    };
}

namespace std {
    template <>
    struct formatter<vespera::core::logger::e_LogLevel>:
        formatter<std::string>
    {
        constexpr auto format(
            const ::vespera::core::logger::e_LogLevel level,
            format_context& ctx
        ) const {
            using vespera::core::logger::e_LogLevel;

            auto&& out = ctx.out();

            // The more common levels should line up in the log,
            // with the more serious ones taking up more space.
            // Info messages should be the most common, so they
            // should be least visually intrusive.
            switch (level) {
                case e_LogLevel::trace:   return format_to(out, "[ - ]");
                case e_LogLevel::debug:   return format_to(out, "[dbg]");
                case e_LogLevel::info:    return format_to(out, "     ");
                case e_LogLevel::warning: return format_to(out, "[***]");
                case e_LogLevel::err:     return format_to(out, "< ERROR >");
                case e_LogLevel::fatal:   return format_to(out, "<## FATAL ##>");
                default:                  return format_to(out, "[ ? ]");
            }
        }
    };
}

#endif
