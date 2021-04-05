// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "Aliases.h"
#include "Reflection.h"

//
// Enumerations
//

// Memory source identifiers. The identifiers are used inside the peek and poke
// lookup tables to indicate the source and target of a peek or poke operation.
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
    
    static bool isValid(long value)
    {
        return value >= 1 && value < M_COUNT;
    }
    
    static const char *prefix() { return "M"; }
    static const char *key(MemoryType value)
    {
        switch (value) {
                
            case M_RAM:     return "M_RAM";
            case M_CHAR:    return "M_CHAR";
            case M_KERNAL:  return "M_KERNAL";
            case M_BASIC:   return "M_BASIC";
            case M_IO:      return "M_IO";
            case M_CRTLO:   return "M_CRTLO";
            case M_CRTHI:   return "M_CRTHI";
            case M_PP:      return "M_PP";
            case M_NONE:    return "M_NONE";
            case M_COUNT:   return "???";
        }
        return "???";
    }
};
#endif

enum_long(RAM_PATTERN)
{
    RAM_PATTERN_C64,
    RAM_PATTERN_C64C,
    RAM_PATTERN_COUNT
};
typedef RAM_PATTERN RamPattern;

#ifdef __cplusplus
struct RamPatternEnum : util::Reflection<RamPatternEnum, RamPattern> {
    
    static bool isValid(long value)
    {
        return (unsigned long)value < RAM_PATTERN_COUNT;
    }
    
    static const char *prefix() { return "RAM_PATTERN"; }
    static const char *key(RamPattern value)
    {
        switch (value) {
                
            case RAM_PATTERN_C64:    return "C64";
            case RAM_PATTERN_C64C:   return "C64C";
            case RAM_PATTERN_COUNT:  return "???";
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
    bool debugcart;
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
