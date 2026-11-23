#ifndef VESPERA_INPUT_BUTTON_H
#define VESPERA_INPUT_BUTTON_H

namespace vespera::input {
    /*
     * We don't compare button states between two frames, but use a latching approach instead
     * - a tap that goes up and down in a single frame should remain 'pressed'
     * - with several simulation steps in one frame, only the first step sees the transition
     */
    struct ButtonState {
        void press();       // ignored while already pressed (key repeat)
        void release();     // ignored while already up
        void clear_edges(); // edges means pressed/released flags

        // NOTE may revisit this in the future, possibly if we're replaying inputs we may want to pack this
        bool m_Down     = false;
        bool m_Pressed  = false;
        bool m_Released = false;
    };
}

#endif