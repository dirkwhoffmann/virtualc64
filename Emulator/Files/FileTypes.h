// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef FILE_TYPES_H
#define FILE_TYPES_H

typedef enum
{    
    UNKNOWN_FILE_FORMAT = 0,
    CRT_FILE,                     // Expansion port cartridge
    V64_FILE,                     // Snapshot
    D64_FILE,                     // Floppy disk
    T64_FILE,                     // Archive with multiple files
    PRG_FILE,                     // Archive with a single file
    P00_FILE,                     // Archive with a single file
    G64_FILE,                     // Bit-stream of a floppy disk
    TAP_FILE,                     // Datasette tape
    BASIC_ROM_FILE,               // Basic Rom
    CHAR_ROM_FILE,                // Character Rom
    KERNAL_ROM_FILE,              // Kernal Rom
    VC1541_ROM_FILE               // Floppy drive Rom
}
C64FileType;

inline bool isFileType(long value)
{
    return value >= 0 && value <= VC1541_ROM_FILE;
}

typedef enum
{
    ROM_MISSING,
    ROM_UNKNOWN,
    
    // Basic Roms
    BASIC_COMMODORE,
    BASIC_MEGA65,

    // Character Roms
    CHAR_COMMODORE,
    CHAR_SWEDISH_C2D007,
    CHAR_SWEDISH,
    CHAR_SPANISH_C64C,
    CHAR_DANISH,
    CHAR_ATARI800,
    CHAR_MSX,
    CHAR_ZX_SPECTRUM,
    CHAR_AMSTRAD_CPC,
    CHAR_A500_TOPAZ_BROKEN,
    CHAR_A500_TOPAZ_V2,
    CHAR_A1200_TOPAZ_BROKEN,
    CHAR_A1200_TOPAZ_V2,
    CHAR_TELETEXT,
    CHAR_MEGA65_GENERIC,

    // Kernal Roms
    KERNAL_1ST,
    KERNAL_2ST,
    KERNAL_3ST,
    KERNAL_DANISH_3RD,
    KERNAL_SX64,
    KERNAL_SX64_SCAND,
    KERNAL_64_JIFFY,
    KERNAL_SX64_JIFFY,
    KERNAL_TURBO_ROM,
    KERNAL_DATEL_V32,
    KERNAL_EXOS_V3,
    KERNAL_TURBO_TAPE,
    KERNAL_MEGA65_GENERIC,

    // VC1541 Roms
    VC1541_II_1987,
    VC1541_II_NEWTRONIC,
    VC1541_OLD_WHITE,
    VC1541_1541C,
    VC1541_II_RELOC_PATCH,
    VC1541_II_JIFFY,
    VC1541_II_JIFFY_V600,
    
    ROM_CNT
}
RomRevision;

inline bool isRomRevision(long value)
{
    return value >= 0 && value <= ROM_CNT;
}

#endif
