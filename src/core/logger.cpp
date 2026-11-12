#include "logger.h"
#include "assert.h"
#include "platform.h"

#include <iostream>
#include <string_view>

namespace vespera::core {
    void log_message(const LogMeta& meta, const std::string& message) {
        // from the path find either the last '/' or '\' and trim it to just the filename
        auto file_name = std::string_view(meta.m_Location.file_name());
        file_name = file_name.substr(
            std::string_view(meta.m_Location.file_name())
                .find_last_of("\\/") + 1
        );

        std::string formatted_message = std::format("{} {}:{}\t{}\n",
                meta.m_Level,
                file_name,
                meta.m_Location.line(),
                message
            );

        std::cerr << formatted_message;

#if VESPERA_PLATFORM == VESPERA_PLATFORM_WINDOWS
        OutputDebugStringA(formatted_message.c_str());
#endif

        VESPERA_CHECK(meta.m_Level != e_LogLevel::fatal, "Fatal error");
    }
}