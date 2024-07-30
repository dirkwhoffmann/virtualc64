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

namespace vc64::util {

// Returns the FNV-1a seed value
inline u32 fnvInit32() { return 0x811c9dc5; }
inline u64 fnvInit64() { return 0xcbf29ce484222325; }

// Performs a single iteration of the FNV-1a hash algorithm
u32 fnvIt32(u32 prv, u32 val);
u64 fnvIt64(u64 prv, u64 val);

// Computes a FNV-1a checksum for a given buffer
u32 fnv32(const u8 *addr, isize size);
u64 fnv64(const u8 *addr, isize size);

// Computes a CRC checksum for a given buffer
u16 crc16(const u8 *addr, isize size);
u32 crc32(const u8 *addr, isize size);
u32 crc32forByte(u32 r);

}
