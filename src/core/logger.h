#ifndef VESPERA_CORE_LOGGER_H
#define VESPERA_CORE_LOGGER_H

#include <source_location>
#include <format>
#include "log_level.h"

namespace vespera::core {
    struct LogMeta {
        std::source_location m_Location;
        e_LogLevel           m_Level = e_LogLevel::info;
    };

    void log_message(const LogMeta& meta, const std::string& message);
}

#define VESPERA_LOG_MACRO(level, ...) vespera::core::log_message( \
    {                                                             \
        .m_Location = std::source_location::current(),            \
        .m_Level    = level                                       \
    },                                                            \
    std::format(__VA_ARGS__)                                      \
)

#define VESPERA_LOG_TRACE(...)   VESPERA_LOG_MACRO(vespera::core::e_LogLevel::trace,   __VA_ARGS__)
#define VESPERA_LOG_DEBUG(...)   VESPERA_LOG_MACRO(vespera::core::e_LogLevel::debug,   __VA_ARGS__)
#define VESPERA_LOG_INFO(...)    VESPERA_LOG_MACRO(vespera::core::e_LogLevel::info,    __VA_ARGS__)
#define VESPERA_LOG_WARNING(...) VESPERA_LOG_MACRO(vespera::core::e_LogLevel::warning, __VA_ARGS__)
#define VESPERA_LOG_ERROR(...)   VESPERA_LOG_MACRO(vespera::core::e_LogLevel::err,     __VA_ARGS__)
#define VESPERA_LOG_FATAL(...)   VESPERA_LOG_MACRO(vespera::core::e_LogLevel::fatal,   __VA_ARGS__)

#define LOG(...) VESPERA_LOG_INFO(__VA_ARGS__)

#endif
