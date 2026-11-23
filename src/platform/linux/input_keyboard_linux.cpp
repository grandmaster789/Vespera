#include "input_keyboard_linux.h"

#if VESPERA_PLATFORM == VESPERA_PLATFORM_LINUX

#include <linux/input-event-codes.h>

namespace vespera::input {
    // This list is incomplete, add entries as needed
    // https://github.com/torvalds/linux/blob/master/include/uapi/linux/input-event-codes.h

    std::optional<e_Key> key_from_evdev_code(std::uint16_t code) {
        switch (code) {
        case KEY_A: return e_Key::a;
        case KEY_B: return e_Key::b;
        case KEY_C: return e_Key::c;
        case KEY_D: return e_Key::d;
        case KEY_E: return e_Key::e;
        case KEY_F: return e_Key::f;
        case KEY_G: return e_Key::g;
        case KEY_H: return e_Key::h;
        case KEY_I: return e_Key::i;
        case KEY_J: return e_Key::j;
        case KEY_K: return e_Key::k;
        case KEY_L: return e_Key::l;
        case KEY_M: return e_Key::m;
        case KEY_N: return e_Key::n;
        case KEY_O: return e_Key::o;
        case KEY_P: return e_Key::p;
        case KEY_Q: return e_Key::q;
        case KEY_R: return e_Key::r;
        case KEY_S: return e_Key::s;
        case KEY_T: return e_Key::t;
        case KEY_U: return e_Key::u;
        case KEY_V: return e_Key::v;
        case KEY_W: return e_Key::w;
        case KEY_X: return e_Key::x;
        case KEY_Y: return e_Key::y;
        case KEY_Z: return e_Key::z;

        case KEY_1: return e_Key::digit_1;
        case KEY_2: return e_Key::digit_2;
        case KEY_3: return e_Key::digit_3;
        case KEY_4: return e_Key::digit_4;
        case KEY_5: return e_Key::digit_5;
        case KEY_6: return e_Key::digit_6;
        case KEY_7: return e_Key::digit_7;
        case KEY_8: return e_Key::digit_8;
        case KEY_9: return e_Key::digit_9;
        case KEY_0: return e_Key::digit_0;

        case KEY_LEFTSHIFT:  return e_Key::left_shift;
        case KEY_RIGHTSHIFT: return e_Key::right_shift;
        case KEY_LEFTCTRL:   return e_Key::left_control;
        case KEY_RIGHTCTRL:  return e_Key::right_control;
        case KEY_LEFTALT:    return e_Key::left_alt;
        case KEY_RIGHTALT:   return e_Key::right_alt;

        case KEY_UP:    return e_Key::up;
        case KEY_DOWN:  return e_Key::down;
        case KEY_LEFT:  return e_Key::left;
        case KEY_RIGHT: return e_Key::right;

        case KEY_F1:  return e_Key::f1;
        case KEY_F2:  return e_Key::f2;
        case KEY_F3:  return e_Key::f3;
        case KEY_F4:  return e_Key::f4;
        case KEY_F5:  return e_Key::f5;
        case KEY_F6:  return e_Key::f6;
        case KEY_F7:  return e_Key::f7;
        case KEY_F8:  return e_Key::f8;
        case KEY_F9:  return e_Key::f9;
        case KEY_F10: return e_Key::f10;
        case KEY_F11: return e_Key::f11;
        case KEY_F12: return e_Key::f12;

        case KEY_ESC:   return e_Key::escape;
        case KEY_SPACE: return e_Key::space;
        case KEY_ENTER: return e_Key::enter;
        case KEY_TAB:   return e_Key::tab;

        default:
            return std::nullopt;
        }
    }
}

#endif