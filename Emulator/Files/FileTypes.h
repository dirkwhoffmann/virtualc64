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

enum_long(FileType)
{    
    FILETYPE_UNKNOWN = 0,
    FILETYPE_V64,
    FILETYPE_CRT,
    FILETYPE_GENERIC_ARCHIVE,
    FILETYPE_T64,
    FILETYPE_P00,
    FILETYPE_PRG,
    FILETYPE_PRG_FOLDER,
    FILETYPE_D64,
    FILETYPE_G64,
    FILETYPE_TAP,
    FILETYPE_BASIC_ROM,
    FILETYPE_CHAR_ROM,
    FILETYPE_KERNAL_ROM,
    FILETYPE_VC1541_ROM
};

inline bool isFileType(long value)
{
    return value >= 0 && value <= FILETYPE_VC1541_ROM;
}

inline const char* fileTypeString(FileType type)
{
    switch (type) {
            
        case FILETYPE_UNKNOWN:    return "UNKNOWN";
        case FILETYPE_V64:        return "V64";
        case FILETYPE_CRT:        return "CRT";
        case FILETYPE_T64:        return "T64";
        case FILETYPE_PRG:        return "PRG";
        case FILETYPE_PRG_FOLDER: return "PRG_FOLDER";
        case FILETYPE_P00:        return "P00";
        case FILETYPE_D64:        return "D64";
        case FILETYPE_G64:        return "G64";
        case FILETYPE_TAP:        return "TAP";
        case FILETYPE_BASIC_ROM:  return "ROM";
        case FILETYPE_CHAR_ROM:   return "ROM";
        case FILETYPE_KERNAL_ROM: return "ROM";
        case FILETYPE_VC1541_ROM: return "ROM";
            
        default: assert(false);
    }
}

enum_long(RomIdentifier)
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

inline bool isRomRevision(long value)
{
    return value >= 0 && value <= ROM_CNT;
}

enum_long(TAPVersion)
{
    TAP_ORIGINAL = 0,
    TAP_ADVANCED = 1
};

inline bool isTAPVersion(long value)
{
    return value >= TAP_ORIGINAL && value <= TAP_ADVANCED;
}

inline const char* sTAPVersion(TAPVersion type)
{
    switch (type) {
            
        case TAP_ORIGINAL: return "TAP_ORIGINAL";
        case TAP_ADVANCED: return "TAP_ADVANCED";
            
        default: assert(false);
    }
}

#endif
