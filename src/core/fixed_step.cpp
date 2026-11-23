#include "fixed_step.h"

#include <cmath>

#include "clean-core/common/assert.hh"

namespace vespera::core {
    FixedStep::FixedStep(
        double step_secs,
        int max_steps_per_advance
    ):
        m_StepSecs(step_secs),
        m_MaxStepsPerAdvance(max_steps_per_advance)
    {
        CC_ASSERT_ALWAYS(step_secs > 0,             "step_secs must be greater than 0");
        CC_ASSERT_ALWAYS(max_steps_per_advance > 0, "The maximum number of steps must be greater than 0");
    }

    int FixedStep::advance(double elapsed_seconds) {
        CC_ASSERT_ALWAYS(elapsed_seconds >= 0, "The elapsed time must not be negative");

        m_BankedTime += elapsed_seconds;

        // apply cap first
        if (m_BankedTime > m_MaxStepsPerAdvance * m_StepSecs) {
            m_BankedTime = 0.0;
            return m_MaxStepsPerAdvance;
        }

        int result = static_cast<int>(std::floor(m_BankedTime / m_StepSecs));
        m_BankedTime -= result * m_StepSecs;

        return result;
    }

    double FixedStep::alpha() const {
        return m_BankedTime / m_StepSecs;
    }
}
