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
#include <bit>

namespace utl {

//
// Bits and bytes
//

// Returns the low nibble or the high nibble of a 8 bit value
#define LO_NIBBLE(x) (u8)((x) & 0xF)
#define HI_NIBBLE(x) (u8)(((x) >> 4) & 0xF)

// Returns the low byte or the high byte of a 16 bit value
#define LO_BYTE(x) (u8)((x) & 0xFF)
#define HI_BYTE(x) (u8)((x) >> 8)

// Returns the low word or the high word of a 32 bit value
#define LO_WORD(x) (u16)((x) & 0xFFFF)
#define HI_WORD(x) (u16)((x) >> 16)

// Constructs a larger integer in little endian byte format
#define LO_HI(x,y) (u16)((y) << 8 | (x))
#define LO_LO_HI(x,y,z) (u32)((z) << 16 | (y) << 8 | (x))
#define LO_LO_HI_HI(x,y,z,w) (u32)((w) << 24 | (z) << 16 | (y) << 8 | (x))
#define LO_W_HI_W(x,y) (u32)((y) << 16 | (x))

// Constructs a larger integer in big endian byte format
#define HI_LO(x,y) (u16)((x) << 8 | (y))
#define HI_HI_LO(x,y,z) (u32)((x) << 16 | (y) << 8 | (z))
#define HI_HI_LO_LO(x,y,z,w) (u32)((x) << 24 | (y) << 16 | (z) << 8 | (w))
#define HI_W_LO_W(x,y) (u32)((x) << 16 | (y))

// Returns a certain byte of a larger integer
#define BYTE0(x) LO_BYTE(x)
#define BYTE1(x) LO_BYTE((x) >> 8)
#define BYTE2(x) LO_BYTE((x) >> 16)
#define BYTE3(x) LO_BYTE((x) >> 24)
#define GET_BYTE(x,nr) LO_BYTE((x) >> (8 * (nr)))

// Returns a non-zero value if the n-th bit is set in x
#define GET_BIT(x,nr) ((x) & (1 << (nr)))

// Sets, clears, or toggles single bits
#define SET_BIT(x,nr) ((x) |= (1 << (nr)))
#define CLR_BIT(x,nr) ((x) &= ~(1 << (nr)))
#define TOGGLE_BIT(x,nr) ((x) ^= (1 << (nr)))

// Replaces bits, bytes, and words
#define REPLACE_BIT(x,nr,v) ((v) ? SET_BIT(x, nr) : CLR_BIT(x, nr))
#define REPLACE_LO(x,y) (((x) & ~0x00FF) | (y))
#define REPLACE_HI(x,y) (((x) & ~0xFF00) | ((y) << 8))
#define REPLACE_LO_WORD(x,y) (((x) & ~0xFFFF) | (y))
#define REPLACE_HI_WORD(x,y) (((x) & ~0xFFFF0000) | ((y) << 16))

// Checks if none or all bits of a bitmask are set
#define ALL_CLR(x,m) (((x) & (m)) == 0)
#define ALL_SET(x,m) (((x) & (m)) == m)

// Checks for a rising or a falling edge
#define RISING_EDGE(x,y) (!(x) && (y))
#define RISING_EDGE_BIT(x,y,n) (!((x) & (1 << (n))) && ((y) & (1 << (n))))
#define FALLING_EDGE(x,y) ((x) && !(y))
#define FALLING_EDGE_BIT(x,y,n) (((x) & (1 << (n))) && !((y) & (1 << (n))))

// Checks is a number is even or odd
#define IS_EVEN(x) (!IS_ODD(x))
#define IS_ODD(x) ((x) & 1)

// Rounds a number up or down to the next even or odd number
#define UP_TO_NEXT_EVEN(x) ((x) + ((x) & 1))
#define DOWN_TO_NEXT_EVEN(x) ((x) & (~1))
#define UP_TO_NEXT_ODD(x) ((x) | 1)
#define DOWN_TO_NEXT_ODD(x) ((x) - !((x) & 1))

// Reads a value in big-endian format
#define R8BE(a)  (*(u8 *)(a))
#define R16BE(a) HI_LO(*(u8 *)(a), *((u8 *)(a)+1))
#define R32BE(a) HI_HI_LO_LO(*(u8 *)(a), *((u8 *)(a)+1), *((u8 *)(a)+2), *((u8 *)(a)+3))

// Writes a value in big-endian format
#define W8BE(a,v)  { *(u8 *)(a) = (v); }
#define W16BE(a,v) { *(u8 *)(a) = HI_BYTE(v); *((u8 *)(a)+1) = LO_BYTE(v); }
#define W32BE(a,v) { W16BE(a,HI_WORD(v)); W16BE((a)+2,LO_WORD(v)); }

// Reverses the byte ordering in an integer value
#ifdef _MSC_VER
#define SWAP16 _byteswap_ushort
#define SWAP32 _byteswap_ulong
#define SWAP64 _byteswap_uint64
#else
#define SWAP16  __builtin_bswap16
#define SWAP32  __builtin_bswap32
#define SWAP64  __builtin_bswap64
#endif


//
// Overflow-prone arithmetic
//

// Sanitizer friendly macros for adding signed offsets to integer values
#define U8_ADD(x,y) (u8)((i64)(x) + (i64)(y))
#define U8_SUB(x,y) (u8)((i64)(x) - (i64)(y))
#define U8_ADD3(x,y,z) (u8)((i64)(x) + (i64)(y) + (i64)(z))
#define U8_SUB3(x,y,z) (u8)((i64)(x) - (i64)(y) - (i64)(z))
#define U8_INC(x,y) x = U8_ADD(x,y)
#define U8_DEC(x,y) x = U8_SUB(x,y)

#define U16_ADD(x,y) (u16)((i64)(x) + (i64)(y))
#define U16_SUB(x,y) (u16)((i64)(x) - (i64)(y))
#define U16_ADD3(x,y,z) (u16)((i64)(x) + (i64)(y) + (i64)(z))
#define U16_SUB3(x,y,z) (u16)((i64)(x) - (i64)(y) - (i64)(z))
#define U16_INC(x,y) x = U16_ADD(x,y)
#define U16_DEC(x,y) x = U16_SUB(x,y)

#define U32_ADD(x,y) (u32)((i64)(x) + (i64)(y))
#define U32_SUB(x,y) (u32)((i64)(x) - (i64)(y))
#define U32_ADD3(x,y,z) (u32)((i64)(x) + (i64)(y) + (i64)(z))
#define U32_SUB3(x,y,z) (u32)((i64)(x) - (i64)(y) - (i64)(z))
#define U32_INC(x,y) x = U32_ADD(x,y)
#define U32_DEC(x,y) x = U32_SUB(x,y)

#define U64_ADD(x,y) (u64)((i64)(x) + (i64)(y))
#define U64_SUB(x,y) (u64)((i64)(x) - (i64)(y))
#define U64_ADD3(x,y,z) (u64)((i64)(x) + (i64)(y) + (i64)(z))
#define U64_SUB3(x,y,z) (u64)((i64)(x) - (i64)(y) - (i64)(z))
#define U64_INC(x,y) x = U64_ADD(x,y)
#define U64_DEC(x,y) x = U64_SUB(x,y)


//
// Byte order
//

// Returns the big endian representation of an integer value
template<typename T> T bigEndian(T x);

template<>
inline u16 bigEndian(u16 x)
{
    if constexpr (std::endian::native == std::endian::big) {
        return x;
    } else {
        return SWAP16(x);
    }
}

template<>
inline u32 bigEndian(u32 x)
{
    if constexpr (std::endian::native == std::endian::big) {
        return x;
    } else {
        return SWAP32(x);
    }
}

template<>
inline u64 bigEndian(u64 x)
{
    if constexpr (std::endian::native == std::endian::big) {
        return x;
    } else {
        return SWAP64(x);
    }
}

// Checks if a certain memory area is all zero
bool isZero(const u8 *ptr, isize size);

// Replaces the first occurence of a byte or character sequence by another one
void replace(u8 *p, isize size, const u8 *sequence, const u8 *substitute);
void replace(char *p, isize size, const char *sequence, const char *substitute);

}
