#include "core/engine.h"
#include "clean-core/record/log.hh"

int main() {
    vespera::core::Engine engine;

    CC_LOG_INFO("Vespera {}", VESPERA_VERSION);

    return 0;
}
