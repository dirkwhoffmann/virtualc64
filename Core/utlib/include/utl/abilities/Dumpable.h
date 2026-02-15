// -----------------------------------------------------------------------------
// This file is part of utlib - A lightweight utility library
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "utl/common.h"
#include <span>
#include <iostream>

namespace utl {

struct DumpOpt
{
    isize base;         // 8 (Oct)  | 10 (Dec)  | 16 (Hex)
};

struct DumpFmt
{
    char   size;        // 'b' (Byte) | 'w' (Word) | 'l' (Long)
    isize  columns;     // Number of data columns
    isize  groups;      // Number of groups to split columns into
    bool   nr;          // Add a column showing the current line number
    bool   offset;      // Add a column showing the current buffer offset
    bool   ascii;       // Add an ASCII column

    string fmt() const; // Translates options to a format string
};

class Dumpable {

public:

    using DataProvider = std::function<optional<isize>(isize,isize)>;

    virtual ~Dumpable() = default;

    // Data reader factories
    static DataProvider dataProvider(const u8 *buf, isize len);
    static DataProvider dataProvider(std::span<const u8> span);

    // The main dump routines
    static void dump(std::ostream &os, const DumpOpt &opt, const DumpFmt &fmt, DataProvider reader);
    static void dump(std::ostream &os, const DumpOpt &opt, const string &fmt, DataProvider reader);

    // Convenience wrappers
    static void hexDump(std::ostream &os, DataProvider reader) {
        dump(os, { .base = 16 }, { .columns = 16, .offset = true, .ascii = false }, reader);
    }
    static void memDump(std::ostream &os, DataProvider reader) {
        dump(os, { .base = 16 }, { .columns = 16, .offset = true, .ascii = true }, reader);
    }
    static void ascDump(std::ostream &os, DataProvider reader) {
        dump(os, { .base = 0 }, { .columns = 64, .offset = true, .ascii = true }, reader);
    }
    static void txtDump(std::ostream &os, DataProvider reader) {
        dump(os, { .base = 0 }, "%a", reader);
    }

    // The data source (must be provided by the subclass)
    virtual DataProvider dataProvider() const = 0;

    // Instance methods
    void dump(std::ostream &os, const DumpOpt &opt, const DumpFmt &fmt) const {
        dump(os, opt, fmt, dataProvider());
    };
    void dump(std::ostream &os, const DumpOpt &opt, const string &fmt) const {
        dump(os, opt, fmt, dataProvider());
    };
    void hexDump(std::ostream &os = std::cout) const { hexDump(os, dataProvider()); }
    void memDump(std::ostream &os = std::cout) const { memDump(os, dataProvider()); }
    void ascDump(std::ostream &os = std::cout) const { ascDump(os, dataProvider()); }
    void txtDump(std::ostream &os = std::cout) const { txtDump(os, dataProvider()); }
};

}
