#include "clean-core/common/assert.hh"
#include "core/engine.h"

int main() {
    vespera::core::Engine engine;
    CC_ASSERT_ALWAYS(false, "This check should fire");
}
