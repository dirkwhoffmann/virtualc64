// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef MEMORY_PUBLIC_H
#define MEMORY_PUBLIC_H

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
    M_NONE
};
typedef M_TYPE MemoryType;

inline bool isMemoryType(long value)
{
    return value >= 1 && value <= M_NONE;
}

inline const char *MemoryTypeName(MemoryType value)
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
    }
    return "???";
}

enum_long(RAM_PATTERN)
{
    RAM_PATTERN_C64,
    RAM_PATTERN_C64C
};
typedef RAM_PATTERN RamPattern;

inline bool isRamPattern(long value)
{
    return (unsigned long)value <= RAM_PATTERN_C64C;
}

inline const char *RamPatternName(RamPattern value)
{
    switch (value) {
            
        case RAM_PATTERN_C64:   return "RAM_PATTERN_C64";
        case RAM_PATTERN_C64C:  return "RAM_PATTERN_C64C";
    }
    return "???";
}

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


#endif
