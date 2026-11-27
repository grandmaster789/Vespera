#ifndef VESPERA_INPUT_KEYBOARD_H
#define VESPERA_INPUT_KEYBOARD_H

#include <array>
#include <cstdint>

#include "button.h"

namespace vespera::input {
    // NOTE this is an incomplete list, add entries as needed
    enum class e_Key:
        std::uint8_t
    {
        a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z,
        digit_0, digit_1, digit_2, digit_3, digit_4, digit_5, digit_6, digit_7, digit_8, digit_9,
        left_shift, right_shift, left_control, right_control, left_alt, right_alt,
        up, down, left, right,
        f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12,
        escape, space, enter, tab,
        count
    };

    struct KeyboardState {
        using KeyList = std::array<ButtonState, static_cast<std::size_t>(e_Key::count)>;

        void clear_all_edges();
        void release_all();

        [[nodiscard]]       ButtonState& operator[](e_Key k);
        [[nodiscard]] const ButtonState& operator[](e_Key k) const;

        KeyList m_Keys;
    };

    // physical key translation is platform specific, see platform/[$]/input_keyboard_[$].[h, cpp]
}

#endif