// -----------------------------------------------------------------------------
// This file is part of Peddle - A MOS 65xx CPU emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Published under the terms of the MIT License
// -----------------------------------------------------------------------------

#include "PeddleTypes.h"
#include "StrWriter.h"

namespace vc64::peddle {

void
StrWriter::sprintd(char *&s, u64 value, isize digits)
{
    for (isize i = digits - 1; i >= 0; i--) {

        u8 digit = value % 10;
        s[i] = '0' + digit;
        value /= 10;
    }
    s += digits;
}

void
StrWriter::sprintd(char *&s, u64 value)
{
    sprintd(s, value, decDigits(value));
}

void
StrWriter::sprintx(char *&s, u64 value, isize digits)
{
    char a = (style.numberFormat.upperCase ? 'A' : 'a') - 10;

    if (value || !style.numberFormat.plainZero) {

        auto prefix = style.numberFormat.prefix;
        for (int i = 0; prefix[i]; i++) *s++ = prefix[i];
    }
    for (isize i = digits - 1; i >= 0; i--) {

        auto digit = char(value % 16);
        s[i] = (digit <= 9) ? ('0' + digit) : (a + digit);
        value /= 16;
    }
    s += digits;
}

void
StrWriter::sprintx(char *&s, u64 value)
{
    sprintx(s, value, hexDigits(value));
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
    auto fill = style.numberFormat.fill;

    switch (style.numberFormat.radix) {

        case 10:
        {
            auto digits = decDigits(value);
            if (fill) for (isize i = 0; i < 3 - digits; i++) *ptr++ = fill;
            sprintd(ptr, value, digits);
            break;
        }
        case 16:
        {
            auto digits = hexDigits(value);
            if (fill) for (isize i = 0; i < 2 - digits; i++) *ptr++ = fill;
            sprintx(ptr, value, digits);
            break;
        }
    }
    
    return *this;
}

StrWriter&
StrWriter::operator<<(u16 value)
{
    auto fill = style.numberFormat.fill;

    switch (style.numberFormat.radix) {

        case 10:
        {
            auto digits = decDigits(value);
            if (fill) for (isize i = 0; i < 5 - digits; i++) *ptr++ = fill;
            sprintd(ptr, value, digits);
            break;
        }
        case 16:
        {
            auto digits = hexDigits(value);
            if (fill) for (isize i = 0; i < 4 - digits; i++) *ptr++ = fill;
            sprintx(ptr, value, digits);
            break;
        }
    }

    return *this;
}

StrWriter&
StrWriter::operator<<(Ins ins)
{
    const char *mnemonic = Peddle::mnemonic[ins.raw];

    *ptr++ = mnemonic[0];
    *ptr++ = mnemonic[1];
    *ptr++ = mnemonic[2];

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
