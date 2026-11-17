#include "init.h"

#include "platform.h"
#include "clean-core/common/log.hh"
#include "clean-core/record/domain.hh"
#include "clean-core/record/console_listener.hh"

namespace vespera {
    CC_REC_DEFINE_DOMAIN(g_rec_domain, "vp");
}

namespace vespera::core {
    void init() {

#if VESPERA_PLATFORM == VESPERA_PLATFORM_WINDOWS
        // this will prevent the death tests from hanging
        // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/set-abort-behavior?view=msvc-180
        _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
#endif

        // enable debug logging for our domain
        // NOTE there are separate flags for debugging and profiling domains
        cc::rec::find_domain("vp")->set_enabled(cc::rec::level::debug, true);

        cc::rec::install_default_console_listener();
    }
}
