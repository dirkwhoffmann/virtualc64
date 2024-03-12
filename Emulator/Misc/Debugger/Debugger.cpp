/// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// This FILE is dual-licensed. You are free to choose between:
//
//     - The GNU General Public License v3 (or any later version)
//     - The Mozilla Public License v2
//
// SPDX-License-Identifier: GPL-3.0-or-later OR MPL-2.0
// -----------------------------------------------------------------------------

#include "config.h"
#include "Debugger.h"
#include "C64.h"
#include "IOUtils.h"

namespace vc64 {

isize
Debugger::dump(char *dst, u16 addr, const char *fmt) const
{
    bool ctrl = false;
    isize bcnt = 0, ccnt = 0;
    char c;

    do {
        c = *fmt++;

        if (!ctrl) {

            if (c == '%') { ctrl = true; } else *dst++ = c;
            continue;
        }

        switch (c) {

            case 'p': // Address

                snprintf(dst, 5, "%04X", addr);
                dst += 4;
                break;

            case 'b': // Byte

                snprintf(dst, 3, "%02X", mem.spypeek(u16(addr + bcnt++)));
                dst += 2;
                break;

            case 'c': // Character
            {
                auto value = mem.spypeek(u16(addr + ccnt++));
                *dst++ = isprint(value) ? char(value) : '.';
                break;
            }
            default:
                fatalError;
        }

        ctrl = false;

    } while (c);

    return std::max(bcnt, ccnt);
}

isize
Debugger::dump(std::ostream& os, u16 addr, const char *fmt) const
{
    char str[256];
    isize result = dump(str, addr, fmt);
    os << str;
    return result;
}

isize
Debugger::ascDump(std::ostream& os, u16 addr, isize lines) 
{
    current = addr;
    
    for (isize i = 0; i < lines; i++) {
        
        current += dump(os, current,
                        "%p: "
                        "%c%c%c%c%c%c%c%c" "%c%c%c%c%c%c%c%c"
                        "%c%c%c%c%c%c%c%c" "%c%c%c%c%c%c%c%c"
                        "%c%c%c%c%c%c%c%c" "%c%c%c%c%c%c%c%c"
                        "%c%c%c%c%c%c%c%c" "%c%c%c%c%c%c%c%c\n");
    }
    
    return isize(current - addr);
}

isize
Debugger::hexDump(std::ostream& os, u16 addr, isize lines)
{
    return 0;
}

isize
Debugger::memDump(std::ostream& os, u16 addr, isize lines)
{
    for (isize i = 0; i < lines; i++) {

        current += dump(os, current,
                        "%p: "
                        "%b %b %b %b %b %b %b %b %b %b %b %b %b %b %b %b  "
                        "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n");
    }

    return isize(current - addr);
}

void
Debugger::convertNumeric(std::ostream& os, u8 value) const
{
    using namespace util;

    os << std::setw(10) << std::right << std::setfill(' ') << dec(value) << " | ";
    os << hex(value) << " | ";
    os << bin(value) << " | ";
    os << str(value);
}

void
Debugger::convertNumeric(std::ostream& os, u16 value) const
{
    using namespace util;

    os << std::setw(10) << std::right << std::setfill(' ') << dec(value) << " | ";
    os << hex(value) << " | ";
    os << bin(value) << " | ";
    os << str(value);
}

void
Debugger::convertNumeric(std::ostream& os, u32 value) const
{
    using namespace util;

    os << std::setw(10) << std::right << std::setfill(' ') << dec(value) << " | ";
    os << hex(value) << " | ";
    os << bin(value) << " | ";
    os << str(value);
}

void
Debugger::convertNumeric(std::ostream& os, string s) const
{
    u8 bytes[4];

    bytes[0] = s.length() >= 4 ? (u8)s[s.length() - 1] : 0;
    bytes[1] = s.length() >= 3 ? (u8)s[s.length() - 2] : 0;
    bytes[2] = s.length() >= 2 ? (u8)s[s.length() - 3] : 0;
    bytes[3] = s.length() >= 1 ? (u8)s[s.length() - 4] : 0;

    convertNumeric(os, u32(HI_HI_LO_LO(bytes[0], bytes[1], bytes[2], bytes[3])));
}

}
