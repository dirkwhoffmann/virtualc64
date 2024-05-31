// -----------------------------------------------------------------------------
// This file is part of Peddle - A MOS 65xx CPU emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Published under the terms of the MIT License
// -----------------------------------------------------------------------------

#pragma once

#include "Peddle.h"
#include <cmath>

namespace vc64::peddle {

//
// Wrapper structures controlling the output format
//

// Mnemonics
struct Ins  { u8 raw; };

// Addressing modes
struct Imm  { u8 raw; };
struct Zp   { u8 raw; };
struct Zpx  { u8 raw; };
struct Zpy  { u8 raw; };
struct Abs  { u16 raw; };
struct Absx { u16 raw; };
struct Absy { u16 raw; };
struct Ind  { u16 raw; };
struct Indx { u8 raw; };
struct Indy { u8 raw; };
struct Rel  { u16 raw; };
struct Dir  { u16 raw; };

// Indentation
struct Tab { };
struct Sep { };

// Finish
struct Fin { };

class StrWriter
{

public:

    char *base;             // Start address of the destination string
    char *ptr;              // Write pointer
    const DasmStyle &style;

public:

    StrWriter(char *p, const DasmStyle &style) : style(style) {

        base = ptr = p;
    };

    isize length() { return isize(ptr - base); }
    void fill(isize tab) { while (ptr < base + tab) *ptr++ = ' '; }

private:

    isize decDigits(u64 value) { return value ? 1 + (isize)log10(value) : 1; }
    isize binDigits(u64 value) { return value ? 1 + (isize)log2(value) : 1; }
    isize hexDigits(u64 value) { return (binDigits(value) + 3) / 4; }

    void sprintd(char *&s, u64 value, isize digits);
    void sprintd(char *&s, u64 value);
    void sprintx(char *&s, u64 value, isize digits);
    void sprintx(char *&s, u64 value);

public:

    StrWriter& operator<<(char);
    StrWriter& operator<<(const char *);
    StrWriter& operator<<(u8);
    StrWriter& operator<<(u16);

    StrWriter& operator<<(Ins);
    StrWriter& operator<<(Imm);
    StrWriter& operator<<(Zp);
    StrWriter& operator<<(Zpx);
    StrWriter& operator<<(Zpy);
    StrWriter& operator<<(Abs);
    StrWriter& operator<<(Absx);
    StrWriter& operator<<(Absy);
    StrWriter& operator<<(Ind);
    StrWriter& operator<<(Indx);
    StrWriter& operator<<(Indy);
    StrWriter& operator<<(Rel);
    StrWriter& operator<<(Dir);

    StrWriter& operator<<(Tab);
    StrWriter& operator<<(Sep);

    StrWriter& operator<<(Fin);
};

}
