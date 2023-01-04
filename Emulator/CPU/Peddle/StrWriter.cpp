// -----------------------------------------------------------------------------
// This file is part of Peddle - A MOS 65xx CPU emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Published under the terms of the MIT License
// -----------------------------------------------------------------------------

#include "PeddleTypes.h"
#include "StrWriter.h"
#include <cmath>

namespace vc64::peddle {

static int decDigits(u64 value) { return value ? 1 + (int)log10(value) : 1; }
static int binDigits(u64 value) { return value ? 1 + (int)log2(value) : 1; }
static int hexDigits(u64 value) { return (binDigits(value) + 3) / 4; }

static void sprintd(char *&s, u64 value, int digits)
{
    for (int i = digits - 1; i >= 0; i--) {

        u8 digit = value % 10;
        s[i] = '0' + digit;
        value /= 10;
    }
    s += digits;
}

static void sprintd(char *&s, u64 value)
{
    sprintd(s, value, decDigits(value));
}

static void sprintd_signed(char *&s, i64 value)
{
    if (value < 0) { *s++ = '-'; value *= -1; }
    sprintd(s, value, decDigits(value));
}

static void sprintx(char *&s, u64 value, const DasmNumberFormat &fmt, int digits)
{
    char a = (fmt.upperCase ? 'A' : 'a') - 10;

    if (value || !fmt.plainZero) {

        for (int i = 0; fmt.prefix[i]; i++) *s++ = fmt.prefix[i];
    }
    for (int i = digits - 1; i >= 0; i--) {

        auto digit = char(value % 16);
        s[i] = (digit <= 9) ? ('0' + digit) : (a + digit);
        value /= 16;
    }
    s += digits;
}

static void sprintx(char *&s, u64 value, const DasmNumberFormat &fmt)
{
    sprintx(s, value, fmt, hexDigits(value));
}

static void sprintx_signed(char *&s, i64 value, const DasmNumberFormat &fmt)
{
    if (value < 0) { *s++ = '-'; value *= -1; }
    sprintx(s, value, fmt, hexDigits(value));
}

static void sprint(char *&s, u64 value, const DasmNumberFormat &fmt)
{
    fmt.radix == 10 ? sprintd(s, value) : sprintx(s, value, fmt);
}

static void sprint_signed(char *&s, i64 value, const DasmNumberFormat &fmt)
{
    fmt.radix == 10 ? sprintd_signed(s, value) : sprintx_signed(s, value, fmt);
}

StrWriter&
StrWriter::operator<<(char c)
{
    *ptr++ = c;
    return *this;
}

StrWriter&
StrWriter::operator<<(const char *str)
{
    while (*str) { *ptr++ = *str++; };
    return *this;
}

StrWriter&
StrWriter::operator<<(u8 value)
{
    if (style.numberFormat.radix == 16) {
        sprintx(ptr, value, style.numberFormat, 2);
    } else {
        sprintd(ptr, value, 3);
    }
    return *this;
}

StrWriter&
StrWriter::operator<<(u16 value)
{
    if (style.numberFormat.radix == 16) {
        sprintx(ptr, value, style.numberFormat, 4);
    } else {
        sprintd(ptr, value, 5);
    }
    return *this;
}

/*
StrWriter&
StrWriter::operator<<(int value)
{
    sprintd_signed(ptr, value);
    return *this;
}

StrWriter&
StrWriter::operator<<(unsigned int value)
{
    sprintd(ptr, value);
    return *this;
}

StrWriter&
StrWriter::operator<<(Int i)
{
    sprint_signed(ptr, i.raw, style.numberFormat);
    return *this;
}

StrWriter&
StrWriter::operator<<(UInt u)
{
    sprint(ptr, u.raw, style.numberFormat);
    return *this;
}

StrWriter&
StrWriter::operator<<(UInt8 u)
{
    if (style.numberFormat.radix == 16) {
        sprintx(ptr, u.raw, style.numberFormat, 2);
    } else {
        sprintd(ptr, u.raw, 3);
    }
    return *this;
}

StrWriter&
StrWriter::operator<<(UInt16 u)
{
    if (style.numberFormat.radix == 16) {
        sprintx(ptr, u.raw, style.numberFormat, 4);
    } else {
        sprintd(ptr, u.raw, 5);
    }
    return *this;
}
*/

/*
StrWriter&
operator<<(Imp imp)
{
    *this << "xxx"; // TODO: << Mnemonic

    return *this;
}

StrWriter&
operator<<(Acc acc)
{
    *this << "xxx"; // TODO: << Mnemonic

    return *this;
}
*/

StrWriter&
StrWriter::operator<<(Ins ins)
{
    *this << ins.raw[0] << ins.raw[1] << ins.raw[2];
    return *this;
}

StrWriter&
StrWriter::operator<<(Imm op)
{
    *this << "#" << op.raw;
    return *this;
}

StrWriter&
StrWriter::operator<<(Zp op)
{
    *this << op.raw;
    return *this;
}

StrWriter&
StrWriter::operator<<(Zpx op)
{
    *this << op.raw << ",X";
    return *this;
}

StrWriter&
StrWriter::operator<<(Zpy op)
{
    *this << op.raw << ",Y";
    return *this;
}

StrWriter&
StrWriter::operator<<(Abs op)
{
    *this << op.raw;
    return *this;
}

StrWriter&
StrWriter::operator<<(Absx op)
{
    *this << op.raw << ",X";
    return *this;
}

StrWriter&
StrWriter::operator<<(Absy op)
{
    *this << op.raw << ",Y";
    return *this;
}

StrWriter&
StrWriter::operator<<(Ind op)
{
    *this << "(" << op.raw << ")";
    return *this;
}

StrWriter&
StrWriter::operator<<(Indx op)
{
    *this << "(" << op.raw << ",X)";
    return *this;
}

StrWriter&
StrWriter::operator<<(Indy op)
{
    *this << "(" << op.raw << "),Y";
    return *this;
}

StrWriter&
StrWriter::operator<<(Rel op)
{
    *this << op.raw;
    return *this;
}

StrWriter&
StrWriter::operator<<(Dir op)
{
    *this << op.raw;
    return *this;
}

StrWriter&
StrWriter::operator<<(Tab tab)
{
    do { *ptr++ = ' '; } while (ptr < base + style.tab);
    return *this;
}

StrWriter&
StrWriter::operator<<(Sep)
{
    *ptr++ = ',';

    /*
    switch (style.syntax) {

        case DASM_MOIRA:
        case DASM_MOIRA_MIT:
        case DASM_MUSASHI:

            *ptr++ = ' ';
            break;

        case DASM_GNU:
        case DASM_GNU_MIT:

            break;
    }
    */

    return *this;
}

StrWriter&
StrWriter::operator<<(Fin)
{
    *ptr = 0;
    return *this;
}

}
