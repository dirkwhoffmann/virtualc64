// -----------------------------------------------------------------------------
// This file is part of utlib - A lightweight utility library
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "utl/abilities/Dumpable.h"
#include "utl/support/Strings.h"

namespace utl {

string
DumpFmt::fmt() const
{
    string fmt;

    string s1 = size == 'l' ? "%l "      :
                size == 'w' ? "%w "      : "%b ";
    string s2 = size == 'l' ? "%c%c%c%c" :
                size == 'w' ? "%c%c"     : "%c";

    if (nr)     fmt += "%n: ";
    if (offset) fmt += "%p: ";

    if (size) {

        isize g = std::max<isize>(1, groups);
        isize base = columns / g;
        isize rest = columns % g;

        for (isize i = 0; i < g; i++) {

            isize count = base + (i < rest ? 1 : 0);
            fmt += repeat(s1, count);

            if (i + 1 < g) fmt += " ";
        }

        fmt += " ";
    }

    if (ascii)
        fmt += "|" + repeat(s2, columns) + "|";

    fmt += "\n";

    return fmt;
}

Dumpable::DataProvider
Dumpable::dataProvider(const u8 *buf, isize len)
{
    return [buf, len](isize offset, isize bytes) -> optional<isize> {

        isize value = 0;

        while (bytes-- > 0) {

            if (offset >= len) return {};
            value = value << 8 | buf[offset++];
        }
        return value;
    };
}

Dumpable::DataProvider
Dumpable::dataProvider(std::span<const u8> span)
{
    return dataProvider(span.data(), span.size());
}

void
Dumpable::dump(std::ostream &os, const DumpOpt &opt, const DumpFmt &fmt, DataProvider reader)
{
    dump(os, opt, fmt.fmt(), reader);
}

void
Dumpable::dump(std::ostream &os, const DumpOpt &opt, const string &fmt, DataProvider reader)
{
    bool ctrl = false;
    int ccnt = 0, bcnt = 0, lcnt = 0, wcnt = 0;
    char c;

    auto out = [&](optional<isize> value, isize size, isize base = 0) {

        base = base ? base : opt.base;
        int w = wcnt;

        if (w == 0) {

            if (base == 8)  w += size == 1 ? 3 : size == 2 ? 6 : 11;
            if (base == 10) w += size == 1 ? 3 : size == 2 ? 5 : 10;
            if (base == 16) w += size == 1 ? 2 : size == 2 ? 4 : 8;
        }

        if (value.has_value()) {

            if (base == 8)  os << std::setw(w) << std::setfill(' ') << std::oct << *value;
            if (base == 10) os << std::setw(w) << std::setfill(' ') << std::dec << *value;
            if (base == 16) os << std::setw(w) << std::setfill('0') << std::hex << *value << std::setfill(' ');

        } else {

            os << std::setw(w) << " ";
        }
    };

    // Continue as long as data is available
    while (reader(bcnt, 1).has_value() && reader(ccnt, 1).has_value()) {

        // Rewind to the beginning of the format string
        const char *p = fmt.c_str();

        // Print one line of data
        while ((c = *p++) != '\0') {

            if (!ctrl) {

                if (c == '%')       { ctrl = true; wcnt = 0; }
                else if (c == '\n') { os << std::endl; lcnt++; }
                else                { os << c; }
                continue;
            }

            if (c >= '0' && c <= '9') {

                wcnt = 10 * wcnt + (c - '0');
                continue;
            }

            switch (c) {

                case 'n': // Line number

                    os << std::setw(wcnt) << std::setfill(' ') << std::dec << lcnt;
                    break;

                case 'p': // Buffer offset

                    out(std::max(bcnt, ccnt), 4, 16);
                    break;

                case 'a': // Character

                    if (auto val = reader(ccnt, 1); val.has_value()) {
                        os << (val == '\n' || isprint(int(*val)) ? char(*val) : ' ');
                        ccnt += 1;
                    } else {
                        os << ' ';
                    }
                    break;

                case 'c': // Character

                    if (auto val = reader(ccnt, 1); val.has_value()) {
                        os << (isprint(int(*val)) ? char(*val) : '.');
                        ccnt += 1;
                    } else {
                        os << ' ';
                    }
                    break;

                case 'b': case '1': // Byte

                    out(reader(bcnt, 1), 1);
                    bcnt += 1;
                    break;

                case 'w': case '2': // Word

                    out(reader(bcnt, 2), 2);
                    bcnt += 2;
                    break;

                case 'l': case '4': // Long

                    out(reader(bcnt, 4), 4);
                    bcnt += 4;
                    break;

                default:
                    fatalError;
            }

            ctrl = false;
        }
    }
}

}
