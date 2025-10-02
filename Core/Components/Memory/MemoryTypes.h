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

#include "Reflection.h"

namespace vc64 {

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

struct MemoryTypeEnum : Reflection<MemoryTypeEnum, MemType> {

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

struct RamPatternEnum : Reflection<RamPatternEnum, RamPattern> {

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

enum class RomType : long
{
    BASIC,
    CHAR,
    KERNAL,
    VC1541
};

struct RomTypeEnum : Reflection<RomTypeEnum, RomType> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = long(RomType::VC1541);

    static const char *_key(RomType value)
    {
        switch (value) {

            case RomType::BASIC:   return "BASIC";
            case RomType::CHAR:    return "CHAR";
            case RomType::KERNAL:  return "KERNAL";
            case RomType::VC1541:  return "VC1541";
        }
        return "???";
    }
    
    static const char *help(RomType value)
    {
        switch (value) {

            case RomType::BASIC:   return "Basic ROM";
            case RomType::CHAR:    return "Character ROM";
            case RomType::KERNAL:  return "Kernal ROM";
            case RomType::VC1541:  return "Floppy Drive ROM";
        }
        return "";
    }
};

enum class RomVendor
{
    UNKNOWN,
    COMMODORE,
    MEGA65,
    OTHER,
};

struct RomVendorEnum : Reflection<RomVendorEnum, RomVendor> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = long(RomVendor::OTHER);

    static const char *_key(RomVendor value)
    {
        switch (value) {

            case RomVendor::UNKNOWN:    return "UNKNOWN";
            case RomVendor::COMMODORE:  return "COMMODORE";
            case RomVendor::MEGA65:     return "MEGA65";
            case RomVendor::OTHER:      return "OTHER";
        }
        return "???";
    }
    
    static const char *help(RomVendor value)
    {
        return "";
    }
};


//
// Structures
//

typedef struct
{
    RamPattern ramPattern;
    bool saveRoms;
    bool heatmap; 
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

    MemType peekSrc[16];
    MemType vicPeekSrc[16];
}
MemInfo;

typedef struct
{
    isize reads[65536];
    isize writes[65536];
}
MemStats;

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

}
