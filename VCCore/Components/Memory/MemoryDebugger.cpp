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

#include "vcconfig.h"
#include "MemoryDebugger.h"
#include "C64.h"
#include "utl/abilities/Dumpable.h"

namespace vc64 {

namespace {

// Dumps a single line of memory. Returns the number of consumed bytes.
isize
dumpLine(const Memory &mem, std::ostream &os, u16 addr, const string &fmt, isize bytes)
{
    auto reader = [&mem, addr, bytes](isize offset, isize n) -> optional<isize> {

        isize value = 0;

        while (n-- > 0) {

            if (offset >= bytes) return {};
            value = value << 8 | mem.spypeek(u16(addr + offset++));
        }
        return value;
    };

    utl::Dumpable::dump(os, { .base = 16, .uppercase = true }, fmt, reader, 0, addr);
    return bytes;
}

string
repeat(const string &unit, isize count, const string &delim = "")
{
    string result;
    for (isize i = 0; i < count; i++) result += (i ? delim : "") + unit;
    return result;
}

}

isize
MemoryDebugger::ascDump(std::ostream &os, u16 addr, isize lines)
{
    current = addr;

    for (isize i = 0; i < lines; i++) {

        current += (u16)dumpLine(mem, os, current, "%4p: " + repeat("%c", 64) + "\n", 64);
    }

    return isize(current - addr);
}

isize
MemoryDebugger::hexDump(std::ostream &os, u16 addr, isize lines)
{
    current = addr;

    for (isize i = 0; i < lines; i++) {

        current += (u16)dumpLine(mem, os, current, "%4p: " + repeat("%b", 16, " ") + "\n", 16);
    }

    return isize(current - addr);
}

isize
MemoryDebugger::memDump(std::ostream &os, u16 addr, isize lines)
{
    current = addr;

    for (isize i = 0; i < lines; i++) {

        current += (u16)dumpLine(mem, os, current, "%4p: " + repeat("%b", 16, " ") + "  " + repeat("%c", 16) + "\n", 16);
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

        mem.poke(addr, u8(val), MemType::RAM);
        
        if (addr == 0xFFFF) break;
    }
}

void
MemoryDebugger::load(fs::path &path, u16 addr)
{
    std::ifstream stream(path, std::ifstream::binary);
    if (!stream.is_open()) throw IOError(IOError::FILE_NOT_FOUND, path);

    load(stream, addr);
}

void
MemoryDebugger::save(std::ostream &os, u16 addr, isize count)
{
    for (isize i = 0; i < count; i++) {

        u16 a = u16(addr + i);
        auto val = mem.peek(a, MemType::RAM);
        os.put(val);

        if (a == 0xFFFF) break;
    }
}

void
MemoryDebugger::save(fs::path &path, u16 addr, isize count)
{
    std::ofstream stream(path, std::ifstream::binary);
    if (!stream.is_open()) throw IOError(IOError::FILE_CANT_CREATE, path);

    save(stream, addr, count);
}

void
MemoryDebugger::convertNumeric(std::ostream &os, u8 value) const
{
    using namespace utl;

    os << std::setw(10) << std::right << std::setfill(' ') << dec(value) << " | ";
    os << hex(value) << " | ";
    os << bin(value) << " | ";
    os << str(value);
}

void
MemoryDebugger::convertNumeric(std::ostream &os, u16 value) const
{
    using namespace utl;

    os << std::setw(10) << std::right << std::setfill(' ') << dec(value) << " | ";
    os << hex(value) << " | ";
    os << bin(value) << " | ";
    os << str(value);
}

void
MemoryDebugger::convertNumeric(std::ostream &os, u32 value) const
{
    using namespace utl;

    os << std::setw(10) << std::right << std::setfill(' ') << dec(value) << " | ";
    os << hex(value) << " | ";
    os << bin(value) << " | ";
    os << str(value);
}

void
MemoryDebugger::convertNumeric(std::ostream &os, string s) const
{
    u8 bytes[4];

    bytes[0] = s.length() >= 4 ? (u8)s[s.length() - 4] : 0;
    bytes[1] = s.length() >= 3 ? (u8)s[s.length() - 3] : 0;
    bytes[2] = s.length() >= 2 ? (u8)s[s.length() - 2] : 0;
    bytes[3] = s.length() >= 1 ? (u8)s[s.length() - 1] : 0;

    convertNumeric(os, u32(HI_HI_LO_LO(bytes[0], bytes[1], bytes[2], bytes[3])));
}

}
