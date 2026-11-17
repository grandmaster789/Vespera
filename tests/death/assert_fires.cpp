#include "clean-core/common/assert.hh"
#include "core/init.h"

int main() {
    vespera::core::init();
    CC_ASSERT_ALWAYS(false, "This check should fire");
}
