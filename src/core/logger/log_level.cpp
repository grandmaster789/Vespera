#include "core/logger/log_level.h"
#include <ostream>

namespace vespera::core::logger {
    std::ostream& operator << (std::ostream& out, e_LogLevel level) {
        out << std::format("[{:>5}] ", level);
        return out;
    }
}