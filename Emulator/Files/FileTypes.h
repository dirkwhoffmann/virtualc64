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
    FileType_UNKNOWN = 0,
    FileType_V64,
    FileType_CRT,
    FileType_GENERIC_ARCHIVE,
    FileType_T64,
    FileType_P00,
    FileType_PRG,
    FileType_FOLDER,
    FileType_D64,
    FileType_G64,
    FileType_TAP,
    FileType_BASIC_ROM,
    FileType_CHAR_ROM,
    FileType_KERNAL_ROM,
    FileType_VC1541_ROM
};

inline bool isFileType(long value)
{
    return value >= 0 && value <= FileType_VC1541_ROM;
}

inline const char *sFileType(FileType type)
{
    switch (type) {
            
        case FileType_UNKNOWN:    return "UNKNOWN";
        case FileType_V64:        return "V64";
        case FileType_CRT:        return "CRT";
        case FileType_T64:        return "T64";
        case FileType_PRG:        return "PRG";
        case FileType_FOLDER:     return "FOLDER";
        case FileType_P00:        return "P00";
        case FileType_D64:        return "D64";
        case FileType_G64:        return "G64";
        case FileType_TAP:        return "TAP";
        case FileType_BASIC_ROM:  return "ROM";
        case FileType_CHAR_ROM:   return "ROM";
        case FileType_KERNAL_ROM: return "ROM";
        case FileType_VC1541_ROM: return "ROM";
            
        default:
            assert(false);
            return "";
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
