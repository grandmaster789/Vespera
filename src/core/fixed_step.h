#ifndef VESPERA_CORE_FIXED_STEP_H
#define VESPERA_CORE_FIXED_STEP_H

namespace vespera::core {
    class FixedStep {
    public:
        explicit FixedStep(
            double step_secs,                // how long a step should take, in seconds
            int    max_steps_per_advance = 8 // the maximum number of steps we can have when advancing
        );

        // Banks elapsed time and returns how many steps are due; at most max_steps_per_advance
        // Time beyond that cap is dropped
        [[nodiscard]] int    advance(double elapsed_seconds);
        [[nodiscard]] double alpha() const; // banked_time / step_sec, in [0, 1)

    private:
        double m_banked_time           = 0.0;  // accumulator starts empty
        double m_step_secs;                    // how long a step should take, in seconds
        int    m_max_steps_per_advance;        // the maximum number of steps we can have when advancing
    };
}

#endif
