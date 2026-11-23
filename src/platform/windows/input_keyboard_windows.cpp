#include "input_keyboard_windows.h"

#if VESPERA_PLATFORM == VESPERA_PLATFORM_WINDOWS

namespace vespera::input {
    // NOTE this is currently an incomplete list, add entries as required
    //
    // https://learn.microsoft.com/en-us/windows/win32/inputdev/about-keyboard-input#scan-codes (Scan 1 make table)
    // https://handmade.network/forums/t/2011-keyboard_inputs_-_scancodes,_raw_input,_text_input,_key_names

    std::optional<e_Key> key_from_win32_scancode(std::uint16_t scancode, bool extended) {
        if (extended)
            scancode = 0xE000 | (scancode & 0xFF);

        switch (scancode) {
            case 0x001E: return e_Key::a;
            case 0x0030: return e_Key::b;
            case 0x002E: return e_Key::c;
            case 0x0020: return e_Key::d;
            case 0x0012: return e_Key::e;
            case 0x0021: return e_Key::f;
            case 0x0022: return e_Key::g;
            case 0x0023: return e_Key::h;
            case 0x0017: return e_Key::i;
            case 0x0024: return e_Key::j;
            case 0x0025: return e_Key::k;
            case 0x0026: return e_Key::l;
            case 0x0032: return e_Key::m;
            case 0x0031: return e_Key::n;
            case 0x0018: return e_Key::o;
            case 0x0019: return e_Key::p;
            case 0x0010: return e_Key::q;
            case 0x0013: return e_Key::r;
            case 0x001F: return e_Key::s;
            case 0x0014: return e_Key::t;
            case 0x0016: return e_Key::u;
            case 0x002F: return e_Key::v;
            case 0x0011: return e_Key::w;
            case 0x002D: return e_Key::x;
            case 0x0015: return e_Key::y;
            case 0x002C: return e_Key::z;

            case 0x0002: return e_Key::digit_1;
            case 0x0003: return e_Key::digit_2;
            case 0x0004: return e_Key::digit_3;
            case 0x0005: return e_Key::digit_4;
            case 0x0006: return e_Key::digit_5;
            case 0x0007: return e_Key::digit_6;
            case 0x0008: return e_Key::digit_7;
            case 0x0009: return e_Key::digit_8;
            case 0x000A: return e_Key::digit_9;
            case 0x000B: return e_Key::digit_0;

            case 0x002A: return e_Key::left_shift;
            case 0x0036: return e_Key::right_shift;
            case 0x001D: return e_Key::left_control;
            case 0xE01D: return e_Key::right_control;
            case 0x0038: return e_Key::left_alt;
            case 0xE038: return e_Key::right_alt;

            case 0xE048: return e_Key::up;
            case 0xE050: return e_Key::down;
            case 0xE04B: return e_Key::left;
            case 0xE04D: return e_Key::right;

            case 0x003B: return e_Key::f1;
            case 0x003C: return e_Key::f2;
            case 0x003D: return e_Key::f3;
            case 0x003E: return e_Key::f4;
            case 0x003F: return e_Key::f5;
            case 0x0040: return e_Key::f6;
            case 0x0041: return e_Key::f7;
            case 0x0042: return e_Key::f8;
            case 0x0043: return e_Key::f9;
            case 0x0044: return e_Key::f10;
            case 0x0057: return e_Key::f11;
            case 0x0058: return e_Key::f12;

            case 0x0001: return e_Key::escape;
            case 0x0039: return e_Key::space;
            case 0x001C: return e_Key::enter;
            case 0x000F: return e_Key::tab;

        default:
            return std::nullopt;
        }
    }
}

#endif