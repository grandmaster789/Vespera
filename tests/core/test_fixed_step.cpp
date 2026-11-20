#include "core/fixed_step.h"
#include "clean-core/common/assert.hh"
#include <cmath>

using vespera::core::FixedStep;

static bool pretty_close(double a, double b) {
    return std::abs(a - b) < 1e-9;
}

int main() {
    FixedStep stepper(0.01);

    // common usage
    CC_ASSERT_ALWAYS(stepper.advance(0.0)   == 0,        "No time advancement should result in no steps");
    CC_ASSERT_ALWAYS(stepper.advance(0.025) == 2,        "2.5 steps should be 2 steps");
    CC_ASSERT_ALWAYS(pretty_close(stepper.alpha(), 0.5), "Previous advance should have banked half a step");
    CC_ASSERT_ALWAYS(stepper.advance(0.005) == 1,        "The banked half step plus one half should be one");
    CC_ASSERT_ALWAYS(pretty_close(stepper.alpha(), 0.0), "Previous advance should leave nothing left in the bank");

    // simulate a 10s stall (window drag, debugger break, etc.) should test the maximum
    CC_ASSERT_ALWAYS(stepper.advance(10.0) == 8,         "Stalls should be capped");
    CC_ASSERT_ALWAYS(pretty_close(stepper.alpha(), 0.0), "Any excess not kept in the bank");
    CC_ASSERT_ALWAYS(stepper.advance(0.0) == 0,          "Excess does not come back either");

    FixedStep capped_stepper(0.01, 3);
    CC_ASSERT_ALWAYS(capped_stepper.advance(1.0) == 3,   "The cap is configurable");
}