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
#include <bitset>

namespace utl {

//
// Extracting lines
//

void head(std::istream &in, std::ostream &out, isize lines);
void tail(std::istream &in, std::ostream &out, isize lines);


//
// Formatters
//

struct dec {

    i64 value;

    dec(i64 v) : value(v) { };
    std::ostream &operator()(std::ostream &os) const;
};

struct hex {

    isize digits;
    u64 value;

    hex(isize d, u64 v) : digits(d), value(v) { };
    hex(u64 v) : hex(16, v) { };
    hex(u32 v) : hex(8, v) { };
    hex(u16 v) : hex(4, v) { };
    hex(u8 v) : hex(2, v) { };
    std::ostream &operator()(std::ostream &os) const;
};

struct bin {

    isize digits;
    u64 value;

    bin(isize d, u64 v) : digits(d), value(v) { };
    bin(u64 v) : bin(64, v) { };
    bin(u32 v) : bin(32, v) { };
    bin(u16 v) : bin(16, v) { };
    bin(u8 v) : bin(8, v) { };
    std::ostream &operator()(std::ostream &os) const;
};

struct flt {

    double value;

    flt(double v) : value(v) { };
    flt(float v) : value(double(v)) { };
    std::ostream &operator()(std::ostream &os) const;
};

struct tab {

    isize pads;
    const string &str;

    tab(isize p, const string &s) : pads(p), str(s) { };
    tab(const string &s) : tab(24, s) { };
    tab() : tab(24, "") { };
    std::ostream &operator()(std::ostream &os) const;
};

struct bol {

    static const string& yes;
    static const string& no;

    bool value;
    const string &str1, &str2;

    bol(bool v, const string &s1, const string &s2) : value(v), str1(s1), str2(s2) { };
    bol(bool v) : bol(v, yes, no) { };
    std::ostream &operator()(std::ostream &os) const;
};

struct str {

    isize characters;
    u64 value;

    str(isize c, u64 v) : characters(c), value(v) { };
    str(u64 v) : str(8, v) { };
    str(u32 v) : str(4, v) { };
    str(u16 v) : str(2, v) { };
    str(u8 v) : str(1, v) { };
    std::ostream &operator()(std::ostream &os) const;
};

inline std::ostream &operator <<(std::ostream &os, dec v) { return v(os); }
inline std::ostream &operator <<(std::ostream &os, hex v) { return v(os); }
inline std::ostream &operator <<(std::ostream &os, bin v) { return v(os); }
inline std::ostream &operator <<(std::ostream &os, flt v) { return v(os); }
inline std::ostream &operator <<(std::ostream &os, tab v) { return v(os); }
inline std::ostream &operator <<(std::ostream &os, bol v) { return v(os); }
inline std::ostream &operator <<(std::ostream &os, str v) { return v(os); }

}
