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

#include "BasicTypes.h"
#include "Roms/RomTypes.h"

namespace vc64 {

// The Rom vocabulary is shared by all cores and lives in rvlib
/// ROM types (see retro::vault::RomType)
using retro::vault::RomType;
using retro::vault::RomTypeEnum;
/// ROM vendors (see retro::vault::RomVendor)
using retro::vault::RomVendor;
using retro::vault::RomVendorEnum;
using retro::vault::RomDate;
/// Converts a ROM date into a string
using retro::vault::RomDateToString;
/// ROM platforms (see retro::vault::RomPlatform)
using retro::vault::RomPlatform;
using retro::vault::RomPlatformEnum;
namespace RomFlags = retro::vault::RomFlags;
using retro::vault::RomTraits;

//
// Enumerations
//

/// Memory type
enum class MemType : long
{
    NONE,         ///< Unmapped
    RAM,          ///< Ram
    CHAR,         ///< Character Rom
    KERNAL,       ///< Kernal Rom
    BASIC,        ///< Basic Rom
    IO,           ///< IO space
    CRTLO,        ///< Cartridge Rom (low bank)
    CRTHI,        ///< Cartridge Rom (high bank)
    PP,           ///< Processor port
};

struct MemoryTypeEnum : utl::Reflectable<MemoryTypeEnum, MemType> {

    static constexpr long minVal = 1;
    static constexpr long maxVal = long(MemType::NONE);

    static const char *_key(MemType value)
    {
        switch (value) {

            case MemType::NONE:    return "NONE";
            case MemType::RAM:     return "RAM";
            case MemType::CHAR:    return "CHAR";
            case MemType::KERNAL:  return "KERNAL";
            case MemType::BASIC:   return "BASIC";
            case MemType::IO:      return "IO";
            case MemType::CRTLO:   return "CRTLO";
            case MemType::CRTHI:   return "CRTHI";
            case MemType::PP:      return "PP";
        }
        return "???";
    }
    
    static const char *help(MemType value)
    {
        return "";
    }
};

/// Ram startup pattern
enum class RamPattern : long
{
    VICE,       ///< Pattern used by the VICE emulator
    CCS,        ///< Pattern used by the CCS emulator
    ZEROES,     ///< Initialize with all zeroes
    ONES,       ///< Initialize with all ones
    RANDOM      ///< Initialize with pseudo-random values
};

struct RamPatternEnum : utl::Reflectable<RamPatternEnum, RamPattern> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = long(RamPattern::RANDOM);

    static const char *_key(RamPattern value)
    {
        switch (value) {

            case RamPattern::VICE:   return "VICE";
            case RamPattern::CCS:    return "CCS";
            case RamPattern::ZEROES: return "ZEROES";
            case RamPattern::ONES:   return "ONES";
            case RamPattern::RANDOM: return "RANDOM";
        }
        return "???";
    }
    
    static const char *help(RamPattern value)
    {
        return "";
    }
};

//
// Structures
//

/// Configuration of the memory
typedef struct
{
    RamPattern ramPattern;                  ///< RAM start-up pattern
    bool saveRoms;                          ///< Include ROMs in snapshots
}
MemConfig;

/// State of the memory
typedef struct
{
    bool exrom;                             ///< State of the EXROM line
    bool game;                              ///< State of the GAME line
    bool loram;                             ///< LORAM bit of the processor port
    bool hiram;                             ///< HIRAM bit of the processor port
    bool charen;                            ///< CHAREN bit of the processor port
    u8   bankMap;                           ///< Current memory bank configuration

    MemType peekSrc[16];                    ///< Memory type read by the CPU from each 4 KB bank
    MemType vicPeekSrc[16];                 ///< Memory type read by the VICII from each 4 KB bank
}
MemInfo;


}
