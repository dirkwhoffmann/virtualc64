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
#include "MemoryDebugger.h"
#include "C64.h"
#include "IOUtils.h"

namespace vc64 {

isize
MemoryDebugger::dump(char *dst, u16 addr, const char *fmt) const
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
MemoryDebugger::dump(std::ostream& os, u16 addr, const char *fmt) const
{
    char str[256];
    isize result = dump(str, addr, fmt);
    os << str;
    return result;
}

isize
MemoryDebugger::ascDump(std::ostream& os, u16 addr, isize lines)
{
    current = addr;
    
    for (isize i = 0; i < lines; i++) {

        current += (u16)dump(os, current,
                             "%p: "
                             "%c%c%c%c%c%c%c%c" "%c%c%c%c%c%c%c%c"
                             "%c%c%c%c%c%c%c%c" "%c%c%c%c%c%c%c%c"
                             "%c%c%c%c%c%c%c%c" "%c%c%c%c%c%c%c%c"
                             "%c%c%c%c%c%c%c%c" "%c%c%c%c%c%c%c%c\n");
    }

    return isize(current - addr);
}

isize
MemoryDebugger::hexDump(std::ostream& os, u16 addr, isize lines)
{
    current = addr;

    for (isize i = 0; i < lines; i++) {

        current += (u16)dump(os, current,
                             "%p: "
                             "%b %b %b %b %b %b %b %b %b %b %b %b %b %b %b %b\n");
    }

    return isize(current - addr);
}

isize
MemoryDebugger::memDump(std::ostream& os, u16 addr, isize lines)
{
    current = addr;

    for (isize i = 0; i < lines; i++) {

        current += (u16)dump(os, current,
                             "%p: "
                             "%b %b %b %b %b %b %b %b %b %b %b %b %b %b %b %b  "
                             "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n");
    }

    return isize(current - addr);
}

isize
MemoryDebugger::memSearch(const string &pattern, u16 addr)
{
    if (isize length = isize(pattern.length()); length > 0) {

        for (isize i = addr; i < 0xFFFF; i++) {

            for (isize j = 0;; j++) {

                // Get a byte from memory
                auto val = mem.spypeek(u16(i + j));

                // Stop searching if we find a mismatch
                if (val != u8(pattern[j])) break;

                // Return true if all values have matched
                if (j == length - 1) { current = u16(i); return i; }
            }
        }
    }

    return -1;
}

void
MemoryDebugger::write(u16 addr, u8 val, isize repeats)
{
    for (isize i = 0; i < repeats && addr + i <= 0xFFFF; i++) {

        mem.poke(u16(addr + i), u8(val));
    }

    current = u16(addr + repeats);
}

void 
MemoryDebugger::copy(u16 src, u16 dst, isize cnt)
{
    if (src < dst) {

        for (isize i = cnt - 1; i >= 0; i--)
            mem.poke(u16(dst + i), mem.spypeek(u16(src + i)));

    } else {

        for (isize i = 0; i <= cnt - 1; i++)
            mem.poke(u16(dst + i), mem.spypeek(u16(src + i)));
    }
}

void
MemoryDebugger::load(std::istream& is, u16 addr)
{
    for (;; addr++) {

        auto val = is.get();
        if (val == EOF) return;

        mem.poke(addr, u8(val), M_RAM);
        
        if (addr == 0xFFFF) break;
    }
}

void
MemoryDebugger::load(fs::path& path, u16 addr)
{
    std::ifstream stream(path, std::ifstream::binary);
    if (!stream.is_open()) throw Error(VC64ERROR_FILE_NOT_FOUND, path);

    load(stream, addr);
}

void
MemoryDebugger::save(std::ostream& os, u16 addr, isize count)
{
    for (isize i = 0; i < count; i++) {

        u16 a = u16(addr + i);
        auto val = mem.peek(a, M_RAM);
        os.put(val);

        if (a == 0xFFFF) break;
    }
}

void
MemoryDebugger::save(fs::path& path, u16 addr, isize count)
{
    std::ofstream stream(path, std::ifstream::binary);
    if (!stream.is_open()) throw Error(VC64ERROR_FILE_CANT_CREATE, path);

    save(stream, addr, count);
}

void
MemoryDebugger::convertNumeric(std::ostream& os, u8 value) const
{
    using namespace util;

    os << std::setw(10) << std::right << std::setfill(' ') << dec(value) << " | ";
    os << hex(value) << " | ";
    os << bin(value) << " | ";
    os << str(value);
}

void
MemoryDebugger::convertNumeric(std::ostream& os, u16 value) const
{
    using namespace util;

    os << std::setw(10) << std::right << std::setfill(' ') << dec(value) << " | ";
    os << hex(value) << " | ";
    os << bin(value) << " | ";
    os << str(value);
}

void
MemoryDebugger::convertNumeric(std::ostream& os, u32 value) const
{
    using namespace util;

    os << std::setw(10) << std::right << std::setfill(' ') << dec(value) << " | ";
    os << hex(value) << " | ";
    os << bin(value) << " | ";
    os << str(value);
}

void
MemoryDebugger::convertNumeric(std::ostream& os, string s) const
{
    u8 bytes[4];

    bytes[0] = s.length() >= 4 ? (u8)s[s.length() - 4] : 0;
    bytes[1] = s.length() >= 3 ? (u8)s[s.length() - 3] : 0;
    bytes[2] = s.length() >= 2 ? (u8)s[s.length() - 2] : 0;
    bytes[3] = s.length() >= 1 ? (u8)s[s.length() - 1] : 0;

    convertNumeric(os, u32(HI_HI_LO_LO(bytes[0], bytes[1], bytes[2], bytes[3])));
}

}
