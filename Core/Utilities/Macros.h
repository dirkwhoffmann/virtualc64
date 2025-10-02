// -----------------------------------------------------------------------------
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

#pragma once

#include "BasicTypes.h"

//
// Optimizing code
//

#ifdef _MSC_VER

#define unreachable    __assume(false)
#define likely(x)      (x)
#define unlikely(x)    (x)

#else

#define unreachable    __builtin_unreachable()
#define likely(x)      __builtin_expect(!!(x), 1)
#define unlikely(x)    __builtin_expect(!!(x), 0)

#endif

#define fatalError     assert(false); unreachable


//
// Accessing bits and bytes
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


//
// Converting data types
//

// Signed alternative for the sizeof keyword
#define isizeof(x) (isize)(sizeof(x))


//
// Converting units
//

// Converts kilo and mega bytes to bytes
#define KB(x) ((x) << 10)
#define MB(x) ((x) << 20)
#define GB(x) ((x) << 30)

// Converts kilo and mega Hertz to Hertz
#define KHz(x) ((x) * 1000)
#define MHz(x) ((x) * 1000000)


//
// Performing overflow-prone arithmetic
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

/* The following macro can be used to disable clang sanitizer checks. It has
 * been added to make the code compatible with gcc which doesn't recognize
 * the 'no_sanitize' keyword.
 */
#if defined(__clang__)
#define NO_SANITIZE(x) __attribute__((no_sanitize(x)))
#else
#define NO_SANITIZE(x)
#endif
