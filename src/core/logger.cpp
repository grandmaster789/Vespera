#include "logger.h"
#include "assert.h"
#include <iostream>
#include <filesystem>

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX
    #include <windows.h>
#endif

namespace vespera::core {
    void log_message(const LogMeta& meta, const std::string& message) {
        std::string formatted_message = std::format("{} {}:{}\t{}\n",
                meta.m_Level,
                std::filesystem::path(meta.m_Location.file_name())
                    .filename().string(),
                meta.m_Location.line(),
                message
            );

        std::cerr << formatted_message;

#ifdef _WIN32
        OutputDebugStringA(formatted_message.c_str());
#endif

        VESPERA_CHECK(meta.m_Level != logger::e_LogLevel::fatal, "Fatal error");
    }
}