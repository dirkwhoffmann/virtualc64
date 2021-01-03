// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef MEMORY_PUBLIC_TYPES_H
#define MEMORY_PUBLIC_TYPES_H

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

enum_long(RAM_PATTERN)
{
    RAM_PATTERN_C64,
    RAM_PATTERN_C64C,
    RAM_PATTERN_COUNT
};
typedef RAM_PATTERN RamPattern;

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
