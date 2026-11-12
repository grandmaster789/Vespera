#ifndef VESPERA_CORE_LOGGER_H
#define VESPERA_CORE_LOGGER_H

#include <source_location>
#include <format>
#include <iostream>

#include "assert.h"
#include "logger/log_level.h"

namespace vespera::core {
    struct LogMeta {
        std::source_location m_Location;
        logger::e_LogLevel   m_Level = logger::e_LogLevel::info;
    };

    inline void log_message(const LogMeta& meta, const std::string& message) {
        std::cerr <<
            std::format("{} {}:{} {}\n",
                meta.m_Level,
                meta.m_Location.file_name(),
                meta.m_Location.line(),
                message
            );

        VESPERA_CHECK(meta.m_Level != logger::e_LogLevel::fatal, "Fatal error");
    }

    template <typename...Ts>
    constexpr std::string format_log_message(
        std::format_string<Ts...> format_message,
        Ts&&... args
    ) {
        return std::format(format_message, std::forward<Ts>(args)...);
    }
}

#define VESPERA_LOG_MACRO(level, ...) vespera::core::log_message( \
    {                                                             \
        .m_Location = std::source_location::current(),            \
        .m_Level    = level                                       \
    },                                                            \
    vespera::core::format_log_message(__VA_ARGS__)                \
)

#define VESPERA_LOG_TRACE(...)   VESPERA_LOG_MACRO(vespera::core::logger::e_LogLevel::trace,   __VA_ARGS__)
#define VESPERA_LOG_DEBUG(...)   VESPERA_LOG_MACRO(vespera::core::logger::e_LogLevel::debug,   __VA_ARGS__)
#define VESPERA_LOG_INFO(...)    VESPERA_LOG_MACRO(vespera::core::logger::e_LogLevel::info,    __VA_ARGS__)
#define VESPERA_LOG_WARNING(...) VESPERA_LOG_MACRO(vespera::core::logger::e_LogLevel::warning, __VA_ARGS__)
#define VESPERA_LOG_ERROR(...)   VESPERA_LOG_MACRO(vespera::core::logger::e_LogLevel::err,     __VA_ARGS__)
#define VESPERA_LOG_FATAL(...)   VESPERA_LOG_MACRO(vespera::core::logger::e_LogLevel::fatal,   __VA_ARGS__)

#define LOG(...) VESPERA_LOG_INFO(__VA_ARGS__)

#endif
