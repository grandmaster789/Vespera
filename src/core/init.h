#ifndef VESPERA_CORE_INIT_H
#define VESPERA_CORE_INIT_H

#include "clean-core/record/domain_fwd.hh"

namespace vespera {
    CC_REC_DECLARE_DOMAIN(g_rec_domain);
}

namespace vespera::core {
    void init(); // initialize clean-core, set abort behavior
}

#endif
