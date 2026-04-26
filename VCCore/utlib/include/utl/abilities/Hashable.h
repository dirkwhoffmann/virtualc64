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

namespace utl {

class Hashable {

public:

    enum class HashAlgorithm { CRC16, CRC32, FNV32, FNV64 };

    //
    // Class methods
    //

    // Returns the FNV-1a seed value
    [[nodiscard]] static consteval u32 fnvInit32() { return 0x811c9dc5; }
    [[nodiscard]] static consteval u64 fnvInit64() { return 0xcbf29ce484222325; }

    // Performs a single iteration of the FNV-1a hash algorithm
    [[nodiscard]] static u32 fnvIt32(u32 prv, u32 val);
    [[nodiscard]] static u64 fnvIt64(u64 prv, u64 val);

    // Computes a checksum for a given buffer
    [[nodiscard]] static u64 hash(const u8 *addr, isize size, HashAlgorithm algorithm);

    // Computes a FNV-1a checksum for a given buffer
    [[nodiscard]] static u32 fnv32(const u8 *addr, isize size);
    [[nodiscard]] static u64 fnv64(const u8 *addr, isize size);

    // Computes a CRC checksum for a given buffer
    [[nodiscard]] static u16 crc16(const u8 *addr, isize size);
    [[nodiscard]] static u32 crc32(const u8 *addr, isize size);

    //
    // Instance methods
    //

    virtual ~Hashable() = default;

    // Main entry point (provided by the subclass)
    [[nodiscard]] virtual u64 hash(HashAlgorithm algorithm) const = 0;

    // Wrapper functions
    [[nodiscard]] u32 fnv32() const { return (u32)hash(HashAlgorithm::FNV32); }
    [[nodiscard]] u64 fnv64() const { return (u64)hash(HashAlgorithm::FNV64); }

    // Computes a CRC checksum for a given buffer
    [[nodiscard]] u16 crc16() const { return (u16)hash(HashAlgorithm::CRC16); }
    [[nodiscard]] u32 crc32() const { return (u32)hash(HashAlgorithm::CRC32); }
};

}
