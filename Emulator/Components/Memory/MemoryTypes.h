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
/// @file

#pragma once

#include "Aliases.h"

//
// Enumerations
//

/// Memory type
enum_long(M_TYPE)
{
    M_RAM = 1,      ///< Ram
    M_CHAR,         ///< Character Rom
    M_KERNAL,       ///< Kernal Rom
    M_BASIC,        ///< Basic Rom
    M_IO,           ///< IO space
    M_CRTLO,        ///< Cartridge Rom (low bank)
    M_CRTHI,        ///< Cartridge Rom (high bank)
    M_PP,           ///< Processor port
    M_NONE,         ///< Unmapped
    M_COUNT
};
typedef M_TYPE MemoryType;

/// Ram startup pattern
enum_long(RAM_PATTERN)
{
    RAM_PATTERN_VICE,       ///< Pattern used by the VICE emulator
    RAM_PATTERN_CCS,        ///< Pattern used by the CCS emulator
    RAM_PATTERN_ZEROES,     ///< Initialize with all zeroes
    RAM_PATTERN_ONES,       ///< Initialize with all ones
    RAM_PATTERN_RANDOM      ///< Initialize with pseudo-random values
};
typedef RAM_PATTERN RamPattern;

enum_long(ROM_TYPE)
{
    ROM_TYPE_BASIC,
    ROM_TYPE_CHAR,
    ROM_TYPE_KERNAL,
    ROM_TYPE_VC1541
};
typedef ROM_TYPE RomType;

enum_long(ROM_VENDOR)
{
    ROM_VENDOR_COMMODORE,
    ROM_VENDOR_MEGA65,
    ROM_VENDOR_OTHER
};
typedef ROM_VENDOR RomVendor;

//
// Structures
//

typedef struct
{
    RamPattern ramPattern;
    bool saveRoms;
}
MemConfig;

typedef struct
{
    bool exrom;
    bool game;
    bool loram;
    bool hiram;
    bool charen;
    u8   bankMap;
    
    MemoryType peekSrc[16];
    MemoryType vicPeekSrc[16];
}
MemInfo;

typedef struct {

    u64 fnv;
    u32 crc;

    const char *title;
    const char *subtitle;
    const char *revision;

    RomVendor vendor;
    RomType type;
    bool patched;
}
RomTraits;
