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

#include "Aliases.h"
#include "Reflection.h"

//
// Enumerations
//

enum_long(M_TYPE)
{
    M_RAM = 1,
    M_CHAR,
    M_KERNAL,
    M_BASIC,
    M_IO,
    M_CRTLO,
    M_CRTHI,
    M_PP,
    M_NONE,
    M_COUNT
};
typedef M_TYPE MemoryType;

#ifdef __cplusplus
struct MemoryTypeEnum : util::Reflection<MemoryTypeEnum, MemoryType> {
    
	static constexpr long minVal = 1;
    static constexpr long maxVal = M_NONE;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }
    
    static const char *prefix() { return "M"; }
    static const char *key(MemoryType value)
    {
        switch (value) {
                
            case M_RAM:     return "RAM";
            case M_CHAR:    return "CHAR";
            case M_KERNAL:  return "KERNAL";
            case M_BASIC:   return "BASIC";
            case M_IO:      return "IO";
            case M_CRTLO:   return "CRTLO";
            case M_CRTHI:   return "CRTHI";
            case M_PP:      return "PP";
            case M_NONE:    return "NONE";
            case M_COUNT:   return "???";
        }
        return "???";
    }
};
#endif

enum_long(RAM_PATTERN)
{
    RAM_PATTERN_VICE,
    RAM_PATTERN_CCS,
    RAM_PATTERN_ZEROES,
    RAM_PATTERN_ONES,
    RAM_PATTERN_RANDOM
};
typedef RAM_PATTERN RamPattern;

#ifdef __cplusplus
struct RamPatternEnum : util::Reflection<RamPatternEnum, RamPattern> {
    
    static constexpr long minVal = 0;
    static constexpr long maxVal = RAM_PATTERN_RANDOM;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }
    
    static const char *prefix() { return "RAM_PATTERN"; }
    static const char *key(RamPattern value)
    {
        switch (value) {
                
            case RAM_PATTERN_VICE:   return "VICE";
            case RAM_PATTERN_CCS:    return "CCS";
            case RAM_PATTERN_ZEROES: return "ZEROES";
            case RAM_PATTERN_ONES:   return "ONES";
            case RAM_PATTERN_RANDOM: return "RANDOM";
        }
        return "???";
    }
};
#endif


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
