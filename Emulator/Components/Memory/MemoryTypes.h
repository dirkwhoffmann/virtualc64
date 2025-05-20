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
enum_long(M_TYPE)
{
    M_NONE,         ///< Unmapped
    M_RAM,          ///< Ram
    M_CHAR,         ///< Character Rom
    M_KERNAL,       ///< Kernal Rom
    M_BASIC,        ///< Basic Rom
    M_IO,           ///< IO space
    M_CRTLO,        ///< Cartridge Rom (low bank)
    M_CRTHI,        ///< Cartridge Rom (high bank)
    M_PP,           ///< Processor port
};
typedef M_TYPE MemoryType;

struct MemoryTypeEnum : util::Reflection<MemoryTypeEnum, MemoryType> {

    static constexpr long minVal = 1;
    static constexpr long maxVal = M_NONE;

    static const char *prefix() { return "M"; }
    static const char *_key(long value)
    {
        switch (value) {

            case M_NONE:    return "NONE";
            case M_RAM:     return "RAM";
            case M_CHAR:    return "CHAR";
            case M_KERNAL:  return "KERNAL";
            case M_BASIC:   return "BASIC";
            case M_IO:      return "IO";
            case M_CRTLO:   return "CRTLO";
            case M_CRTHI:   return "CRTHI";
            case M_PP:      return "PP";
        }
        return "???";
    }
    
    static const char *help(MemoryType value)
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

struct RamPatternEnum : util::Reflection<RamPatternEnum, RamPattern> {

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

struct RomTypeEnum : util::Reflection<RomTypeEnum, RomType> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = long(RomType::VC1541);

    static const char *prefix() { return "ROM_TYPE"; }
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

enum_long(ROM_VENDOR)
{
    ROM_VENDOR_COMMODORE,
    ROM_VENDOR_MEGA65,
    ROM_VENDOR_OTHER
};
typedef ROM_VENDOR RomVendor;

struct RomVendorEnum : util::Reflection<RomVendorEnum, RomVendor> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = ROM_VENDOR_OTHER;

    static const char *prefix() { return "ROM_VENDOR"; }
    static const char *_key(long value)
    {
        switch (value) {

            case ROM_VENDOR_COMMODORE:  return "COMMODORE";
            case ROM_VENDOR_MEGA65:     return "MEGA65";
            case ROM_VENDOR_OTHER:      return "OTHER";
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

    MemoryType peekSrc[16];
    MemoryType vicPeekSrc[16];
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
