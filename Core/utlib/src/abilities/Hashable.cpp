// -----------------------------------------------------------------------------
// This file is part of utlib - A lightweight utility library
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "utl/abilities/Hashable.h"
#include <array>

namespace utl {

u32 NO_SANITIZE("unsigned-integer-overflow")
Hashable::fnvIt32(u32 prv, u32 val)
{
    return (prv ^ val) * 0x1000193;
}

u64 NO_SANITIZE("unsigned-integer-overflow")
Hashable::fnvIt64(u64 prv, u64 val)
{
    return (prv ^ val) * 0x100000001b3;
}

u64
Hashable::hash(const u8 *addr, isize size, HashAlgorithm algorithm)
{
    switch (algorithm) {

        case HashAlgorithm::FNV32: return fnv32(addr, size);
        case HashAlgorithm::FNV64: return fnv64(addr, size);
        case HashAlgorithm::CRC16: return crc16(addr, size);
        case HashAlgorithm::CRC32: return crc32(addr, size);

        default:
            fatalError;
    }
}

u32
Hashable::fnv32(const u8 *addr, isize size)
{
    if (addr == nullptr || size == 0) return 0;

    u32 hash = fnvInit32();

    for (isize i = 0; i < size; i++) {
        hash = fnvIt32(hash, (u32)addr[i]);
    }

    return hash;
}

u64
Hashable::fnv64(const u8 *addr, isize size)
{
    if (addr == nullptr || size == 0) return 0;

    u64 hash = fnvInit64();

    for (isize i = 0; i < size; i++) {
        hash = fnvIt64(hash, (u64)addr[i]);
    }

    return hash;
}

u16
Hashable::crc16(const u8 *addr, isize size)
{
    if (addr == nullptr || size == 0) return 0;

    u8 x;
    u16 crc = 0xFFFF;

    while (size--){
        x = crc >> 8 ^ *addr++;
        x ^= x>>4;
        crc = (u16)((crc << 8) ^ ((u16)(x << 12)) ^ ((u16)(x <<5)) ^ ((u16)x));
    }
    return crc;
}

u32
Hashable::crc32(const u8 *addr, isize size)
{
    if (addr == nullptr || size == 0) return 0;

    // Compute CRC-32 table
    static const std::array<u32, 256> table = []{
        std::array<u32, 256> t{};
        for (u32 i = 0; i < 256; ++i) {
            u32 r = i;
            for (int j = 0; j < 8; ++j)
                r = (r & 1 ? 0 : 0xEDB88320) ^ (r >> 1);
            t[i] = r ^ 0xFF000000;
        }
        return t;
    }();

    // Compute CRC-32 checksum
    u32 result = 0;
    for(isize i = 0; i < size; i++)
        result = table[(u8)result ^ addr[i]] ^ result >> 8;

    return result;
}

}
