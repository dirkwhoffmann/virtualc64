// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef FILE_PUBLIC_H
#define FILE_PUBLIC_H

enum_long(FILETYPE)
{    
    FILETYPE_UNKNOWN = 0,
    FILETYPE_V64,
    FILETYPE_CRT,
    FILETYPE_T64,
    FILETYPE_P00,
    FILETYPE_PRG,
    FILETYPE_FOLDER,
    FILETYPE_D64,
    FILETYPE_G64,
    FILETYPE_TAP,
    FILETYPE_BASIC_ROM,
    FILETYPE_CHAR_ROM,
    FILETYPE_KERNAL_ROM,
    FILETYPE_VC1541_ROM
};
typedef FILETYPE FileType;

enum_long(ROM_IDENTIFIER)
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
    CHAR_MEGA65,
    CHAR_PXLFONT_V23,

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
    KERNAL_MEGA65,

    // VC1541 Roms
    VC1541_II_1987,
    VC1541_II_NEWTRONIC,
    VC1541_OLD_WHITE,
    VC1541_1541C,
    VC1541_II_RELOC_PATCH,
    VC1541_II_JIFFY,
    VC1541_II_JIFFY_V600,
    
    ROM_CNT
};
typedef ROM_IDENTIFIER RomIdentifier;

enum_long(FileError)
{
    ERR_FILE_OK,
    
    // General
    ERR_FILE_NOT_FOUND,
    ERR_INVALID_TYPE,
    ERR_CANT_READ,
    ERR_CANT_WRITE,
    ERR_OUT_OF_MEMORY,
    ERR_UNKNOWN,
    
    // Snapshots
    ERR_UNSUPPORTED_SNAPSHOT    
};

enum_long(TAP_VERSION)
{
    TAP_VERSION_ORIGINAL,
    TAP_VERSION_ADVANCED
};
typedef TAP_VERSION TAPVersion;

#endif
