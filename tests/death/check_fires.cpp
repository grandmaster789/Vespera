#include <core/assert.h>

int main() {
    VESPERA_ASSERT(false, "This assert should fire");
    //VESPERA_CHECK(false, "This check should fire");
}