// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "Types.h"
#include "Checksum.h"
#include <bit>

namespace util {

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
// Byte order
//

// Returns the big endian representation of an integer value
template<typename T> T bigEndian(T x);

template<>
inline uint16_t bigEndian(uint16_t x)
{
    if constexpr (std::endian::native == std::endian::big) {
        return x;
    } else {
        return SWAP16(x);
    }
}

template<>
inline uint32_t bigEndian(uint32_t x)
{
    if constexpr (std::endian::native == std::endian::big) {
        return x;
    } else {
        return SWAP32(x);
    }
}

template<>
inline uint64_t bigEndian(uint64_t x)
{
    if constexpr (std::endian::native == std::endian::big) {
        return x;
    } else {
        return SWAP64(x);
    }
}

//
// Memory content
//

// Reads a value in big-endian format
#define R8BE(a)  (*(u8 *)(a))
#define R16BE(a) HI_LO(*(u8 *)(a), *(u8 *)((a)+1))
#define R32BE(a) HI_HI_LO_LO(*(u8 *)(a), *(u8 *)((a)+1), *(u8 *)((a)+2), *(u8 *)((a)+3))

#define R8BE_ALIGNED(a)     (*(u8 *)(a))
#define R16BE_ALIGNED(a)    (util::bigEndian(*(u16 *)(a)))
#define R32BE_ALIGNED(a)    (util::bigEndian(*(u32 *)(a)))

// Writes a value in big-endian format
#define W8BE(a,v)  { *(u8 *)(a) = (v); }
#define W16BE(a,v) { *(u8 *)(a) = HI_BYTE(v); *(u8 *)((a)+1) = LO_BYTE(v); }
#define W32BE(a,v) { W16BE(a,HI_WORD(v)); W16BE((a)+2,LO_WORD(v)); }

#define W8BE_ALIGNED(a,v)   { *(u8 *)(a) = (u8)(v); }
#define W16BE_ALIGNED(a,v)  { *(u16 *)(a) = util::bigEndian((u16)v); }
#define W32BE_ALIGNED(a,v)  { *(u32 *)(a) = util::bigEndian((u32)v); }

// Checks if a certain memory area is all zero
bool isZero(const u8 *ptr, isize size);

// Replaces the first occurence of a byte or character sequence by another one
void replace(u8 *p, isize size, const u8 *sequence, const u8 *substitute);
void replace(char *p, isize size, const char *sequence, const char *substitute);

// Extracts all readable ASCII characters from a buffer
void readAscii(const u8 *buf, isize len, char *result, char fill = '.');

// Prints a hex dump of a buffer to the console
void hexdump(u8 *p, isize size, isize cols, isize pad);
void hexdump(u8 *p, isize size, isize cols = 32);
void hexdumpWords(u8 *p, isize size, isize cols = 32);
void hexdumpLongwords(u8 *p, isize size, isize cols = 32);

}
