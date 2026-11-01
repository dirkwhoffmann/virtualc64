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
StrWriter::sprintx(char *&s, u64 value, isize digits, isize width, char fill)
{
    char a = (format.upperCase ? 'A' : 'a') - 10;

    if (value || !format.plainZero) {

        auto prefix = format.prefix;
        for (int i = 0; prefix[i]; i++) *s++ = prefix[i];
    }

    // Pad after the prefix (so that the result looks like $0A, not 0$A)
    if (fill) for (isize i = 0; i < width - digits; i++) *s++ = fill;

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
StrWriter::operator<<(const string &str)
{
    return *this << str.c_str();
}

StrWriter&
StrWriter::operator<<(string_view str)
{
    std::memcpy(ptr, str.data(), str.size());
    ptr += str.size();
    return *this;
}

StrWriter&
StrWriter::operator<<(u8 value)
{
    auto fill = format.fill;

    switch (format.radix) {

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
            sprintx(ptr, value, digits, 2, fill);
            break;
        }
    }
    
    return *this;
}

StrWriter&
StrWriter::operator<<(u16 value)
{
    auto fill = format.fill;

    switch (format.radix) {

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
            sprintx(ptr, value, digits, 4, fill);
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
    if (auto symbol = resolveSymbol(op.raw); symbol) {
        *this << *symbol;
    } else {
        *this << op.raw;
    }
    return *this;
}

StrWriter&
StrWriter::operator<<(Zpx op)
{
    if (auto symbol = resolveSymbol(op.raw); symbol) {
        *this << *symbol << ",X";
    } else {
        *this << op.raw << ",X";
    }
    return *this;
}

StrWriter&
StrWriter::operator<<(Zpy op)
{
    if (auto symbol = resolveSymbol(op.raw); symbol) {
        *this << *symbol << ",Y";
    } else {
        *this << op.raw << ",Y";
    }
    return *this;
}

StrWriter&
StrWriter::operator<<(Abs op)
{
    if (auto symbol = resolveSymbol(op.raw); symbol) {
        *this << *symbol;
    } else {
        *this << op.raw;
    }
    return *this;
}

StrWriter&
StrWriter::operator<<(Absx op)
{
    if (auto symbol = resolveSymbol(op.raw); symbol) {
        *this << *symbol << ",X";
    } else {
        *this << op.raw << ",X";
    }
    return *this;
}

StrWriter&
StrWriter::operator<<(Absy op)
{
    if (auto symbol = resolveSymbol(op.raw); symbol) {
        *this << *symbol << ",Y";
    } else {
        *this <<  op.raw << ",Y";
    }
    return *this;
}

StrWriter&
StrWriter::operator<<(Ind op)
{
    if (auto symbol = resolveSymbol(op.raw); symbol) {
        *this << "(" << *symbol << ")";
    } else {
        *this << "(" << op.raw << ")";
    }
    return *this;
}

StrWriter&
StrWriter::operator<<(Indx op)
{
    if (auto symbol = resolveSymbol(op.raw); symbol) {
        *this << "(" << *symbol << ",X)";
    } else {
        *this << "(" << op.raw << ",X)";
    }
    return *this;
}

StrWriter&
StrWriter::operator<<(Indy op)
{
    if (auto symbol = resolveSymbol(op.raw); symbol) {
        *this << "(" << *symbol << "),Y";
    } else {
        *this << "(" << op.raw << "),Y";
    }
    return *this;
}

StrWriter&
StrWriter::operator<<(Rel op)
{
    if (auto symbol = resolveSymbol(op.raw); symbol) {
        *this << *symbol;
    } else {
        *this << op.raw;
    }
    return *this;
}

StrWriter&
StrWriter::operator<<(Dir op)
{
    if (auto symbol = resolveSymbol(op.raw); symbol) {
        *this << *symbol;
    } else {
        *this << op.raw;
    }
    return *this;
}

StrWriter&
StrWriter::operator<<(Tab tab)
{
    do { *ptr++ = ' '; } while (ptr < base + column);
    return *this;
}

StrWriter&
StrWriter::operator<<(Sep)
{
    *ptr++ = ',';
    return *this;
}

StrWriter&
StrWriter::operator<<(Fin)
{
    *ptr = 0;
    return *this;
}

}
