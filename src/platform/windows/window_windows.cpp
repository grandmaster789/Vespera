#include "../platform.h"

#if VESPERA_PLATFORM == VESPERA_PLATFORM_WINDOWS
#include "platform_windows.h"
#include "../window.h"
#include "typed-geometry/linalg/vec.hh"

namespace vespera::platform {
    struct Window::Impl {
        ~Impl() {
            if (m_Handle)
                DestroyWindow(m_Handle);
        }

        LRESULT message_pump(
            HWND   hwnd,
            UINT   msg,
            WPARAM wParam,
            LPARAM lParam
        ) {
            if (hwnd == m_Handle) {
                switch (msg) {
                case WM_PAINT:
                    break;

                default:
                    return DefWindowProc(hwnd, msg, wParam, lParam);
                }
            }

            return 0;
        }

        static LRESULT WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
            // https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getwindowlongptra
            LONG_PTR userdata = GetWindowLongPtr(hwnd, GWLP_USERDATA);

            if (userdata == NULL)
                return DefWindowProc(hwnd, msg, wParam, lParam);

            return reinterpret_cast<Impl*>(userdata)->message_pump(hwnd, msg, wParam, lParam);
        }

        HWND m_Handle;
    };

    std::expected<Window, std::string> Window::create(const WindowDesc& desc) {
        // https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setthreaddpiawarenesscontext
        SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

        // Create a game-style window without a title bar or menu bar and such
        auto application_handle = GetModuleHandle(nullptr);

        static const ATOM s_class = register_window_class();

        WNDCLASSEX wc = {};
        wc.cbSize        = sizeof(WNDCLASSEX);
        wc.style         = CS_DBLCLKS;                         // https://docs.microsoft.com/en-us/windows/win32/winmsg/window-class-styles
        wc.lpfnWndProc   = Impl::WndProc;                      // A pointer to the window procedure
        wc.cbClsExtra    = 0;                                  // The number of extra bytes to allocate following the window-class structure
        wc.cbWndExtra    = 0;                                  // The number of extra bytes to allocate following the window instance
        wc.hInstance     = application_handle;                 // A handle to the instance that contains the window procedure for the class
        wc.hIcon         = LoadIcon(nullptr, IDI_APPLICATION); // https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-loadicona
        wc.hIconSm       = LoadIcon(nullptr, IDI_WINLOGO);     // A handle to a small icon that is associated with the window class
        wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);     // https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-loadcursora
        wc.hbrBackground = nullptr;                            // A handle to the class background brush
        wc.lpszMenuName  = nullptr;                            // The resource name of the class menu, as the name appears in the resource file
        wc.lpszClassName = L"VesperaWindowClass";              // A pointer to a null-terminated string or an atom

        if (RegisterClassEx(&wc) == 0)
            return std::unexpected("Failed to register WindowClass");

        DWORD window_style =
            WS_CLIPSIBLINGS |
            WS_CLIPCHILDREN |
            WS_SYSMENU |
            WS_MINIMIZEBOX;

        DWORD window_style_ex =
            WS_EX_APPWINDOW;

        // Adjust indicated width/height to take DPI into account
        // https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-adjustwindowrectexfordpi
        RECT dims = {
            .left   = 0,
            .top    = 0,
            .right  = desc.m_Width,
            .bottom = desc.m_Height
        };

        auto dpi = GetDpiForSystem();

        if (!AdjustWindowRectExForDpi(
            &dims,            // lpRect
            window_style,     // dwStyle
            FALSE,            // bMenu
            window_style_ex,  // dwExStyle
            dpi               // dpi
        ))
            return std::unexpected("Failed to adjust window rectangle for DPI");

        auto title = std::string(desc.m_Title);

        // https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createwindowexa
        auto hwnd = CreateWindowEx(
            window_style_ex,
            wc.lpszClassName,
            title.c_str(),
            window_style,
            CW_USEDEFAULT,          // x (left)
            CW_USEDEFAULT,          // y (top)
            dims.right - dims.left, // width
            dims.bottom - dims.top, // height
            nullptr,                // parent window handle
            nullptr,                // menu handle
            application_handle,     // program instance
            nullptr                 // lpParam
        );

        if (!hwnd)
            return std::unexpected("Failed to create Window");

        auto impl = std::make_unique<Impl>();
        impl->m_Handle = hwnd;

        // set the userdata associated with the native window handle,
        // so that the static window message pump can forward to this specific instance
        SetWindowLongPtr(
            hwnd,
            GWLP_USERDATA,
            reinterpret_cast<LONG_PTR>(impl.get())
        );

        return Window(std::move(impl));
    }

    Window::Window(std::unique_ptr<Impl>&& impl):
        m_Impl(std::move(impl))
    {
    }
    Window::~Window() = default;
    Window::Window(Window&&) noexcept = default;
    Window& Window::operator = (Window&&) noexcept = default;

    void Window::poll_events(input::InputState& is) {
        (void)is;
    }

    bool Window::close_requested() const {
        return {};
    }

    tg::vec2i Window::client_size() const {
        return {};
    }

    NativeWindow Window::native() const {
        return {};
    }
}

#endif
