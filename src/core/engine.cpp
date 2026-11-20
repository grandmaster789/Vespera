#include "engine.h"

#include "platform/platform.h"
#include "clean-core/common/log.hh"
#include "clean-core/record/domain.hh"
#include "clean-core/record/console_listener.hh"
#include "clean-core/record/system.hh"

namespace {
    cc::rec::listener_handle g_vespera_console_listener;
}

namespace vespera {
    CC_REC_DEFINE_DOMAIN(g_vespera_domain, "vp");
}

namespace vespera::core {
    Engine::Engine() {
        init();
    }

    Engine::~Engine() {
        shutdown();
    }

    void Engine::init() {

#if VESPERA_PLATFORM == VESPERA_PLATFORM_WINDOWS
        // this will prevent the death tests from hanging
        // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/set-abort-behavior?view=msvc-180
        _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
#endif

        // enable debug logging for our domain
        // NOTE there are separate flags for debugging and profiling domains
        g_vespera_domain.set_enabled(cc::rec::level::debug, true);

        g_vespera_console_listener = cc::rec::install_default_console_listener();
    }

    void Engine::shutdown() {
        cc::rec::unregister_listener(g_vespera_console_listener);
        cc::rec::shutdown();
    }
}
