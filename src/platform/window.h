#ifndef VESPERA_PLATFORM_WINDOW_H
#define VESPERA_PLATFORM_WINDOW_H

#include <expected>
#include <memory>
#include <string>
#include <string_view>
#include <cstdint>

#include "typed-geometry/linalg/fwd.hh"

namespace vespera::input {
    struct InputState;
}

namespace vespera::platform {
    struct WindowDesc {
        std::string_view m_Title  = "Vespera";
        int              m_Width  = 1280;
        int              m_Height = 720;
    };

    struct NativeWindow {
        void*         m_Hwnd          = nullptr; // Windows only HWND
        void*         m_XcbConnection = nullptr; // Linux only
        std::uint32_t m_XcbWindow     = 0;       // only used on Linux
    };

    class Window {
    public:
        static std::expected<Window, std::string> create(const WindowDesc& desc);
        ~Window();

        Window             (Window&&) noexcept;
        Window& operator = (Window&&) noexcept;
        Window             (const Window&) = delete;
        Window& operator = (const Window&) = delete;

                      void         poll_events(input::InputState& is); // drains the OS queue without blocking
        [[nodiscard]] bool         close_requested() const;
        [[nodiscard]] tg::vec2i    client_size() const;
        [[nodiscard]] NativeWindow native() const;

    private:
        struct Impl;
        explicit Window(std::unique_ptr<Impl>&& impl);
        std::unique_ptr<Impl> m_Impl;
    };
}

#endif
