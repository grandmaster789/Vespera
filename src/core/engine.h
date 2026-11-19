#ifndef VESPERA_CORE_ENGINE_H
#define VESPERA_CORE_ENGINE_H

#include "clean-core/record/domain_fwd.hh"

namespace vespera {
    CC_REC_DECLARE_DOMAIN(g_vespera_domain);
}

namespace vespera::core {
    class Engine {
    public:
        Engine();
        ~Engine();

        Engine             (const Engine&) = delete;
        Engine& operator = (const Engine&) = delete;
        Engine             (Engine&&) noexcept = delete;
        Engine& operator = (Engine&&) noexcept = delete;

    private:
        void init();     // initialize external libraries, set abort behavior
        void shutdown(); // shutdown external libraries
    };
}

#endif
