#ifndef VESPERA_CORE_PLATFORM_H
#define VESPERA_CORE_PLATFORM_H

// main platform-specific dispatch per platform

#define VESPERA_PLATFORM_WINDOWS 1
#define VESPERA_PLATFORM_LINUX 2

#ifdef _WIN32
    #define VESPERA_PLATFORM VESPERA_PLATFORM_WINDOWS
#elif defined(__linux__)
    #define VESPERA_PLATFORM VESPERA_PLATFORM_LINUX
#else
    #error "Unsupported platform"
#endif

namespace vespera::core {
    enum class e_Platform {
        windows = VESPERA_PLATFORM_WINDOWS,
        linux   = VESPERA_PLATFORM_LINUX
    };

    constexpr e_Platform platform   = static_cast<e_Platform>(VESPERA_PLATFORM);
    constexpr bool       is_windows = platform == e_Platform::windows;
    constexpr bool       is_linux   = platform == e_Platform::linux;

    bool is_debugger_attached();
    void break_if_debugger_attached();
    void debug_output(const char* message);
}

#endif