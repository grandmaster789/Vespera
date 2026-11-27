#include <platform/window.h>
#include <input/input.h>
#include <core/engine.h>
#include "clean-core/common/assert.hh"

int main() {
    using vespera::core::Engine;
    using vespera::input::InputState;
    using vespera::platform::Window;

    Engine engine;

    auto win = Window::create({
        .m_Title  = "Test",
        .m_Width  = 320,
        .m_Height = 240
    });
    CC_ASSERT_ALWAYS(win.has_value(), "Window creation should succeed");

    InputState is;
    for (int i = 0; i < 10; ++i)
        win->poll_events(is);
    CC_ASSERT_ALWAYS(!win->close_requested(), "Window should keep open");

    return 0;
}