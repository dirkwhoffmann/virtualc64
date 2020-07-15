// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"

const u8 C64RomFile::magicBasicRomBytes[basicRomSignatureCnt][3] = {

    { 0x94, 0xE3, 0x7B },       // Commodore ROM
    { 0xB9, 0xA7, 0xC1 },       // MEGA65 project
    { 0x63, 0xA6, 0xC1 }        // MEGA65 project

};

const u8 C64RomFile::magicCharRomBytes[charRomSignatureCnt][4] = {

    { 0x3C, 0x66, 0x6E, 0x6E }, // Commodore ROM
    { 0x00, 0x3C, 0x66, 0x6E }, // chargen-atari800      (1130C1CE287876DD)
    { 0x70, 0x88, 0x08, 0x68 }, // chargen-msx           (975546A5B6168FFD)
    { 0x00, 0x3C, 0x4A, 0x56 }, // chargen-speccy        (7C74107C9365F735)
    { 0x7C, 0xC6, 0xDE, 0xDE }, // chargen-amstradcpc    (AFFE8B0EE2176CBD)
    { 0x7C, 0xC6, 0xDE, 0xDE }, // Amiga 500 Topaz       (D14C5BE4FEE17705)
    { 0x7C, 0xC6, 0xDE, 0xDE }, // Amiga 500 Topaz V2    (A2C6A6E2C0477981)
    { 0x7C, 0xC6, 0xDE, 0xD6 }, // Amiga 1200 Topaz      (3BF55C821EE80365)
    { 0x7C, 0xC6, 0xDE, 0xD6 }, // Amiga 1200 Topaz V2   (19F0DD3F3F9C4FE9)
    { 0x38, 0x44, 0x5C, 0x54 }, // Teletext              (E527AD3E0DDE930D)
};

const u8 C64RomFile::magicKernalRomBytes[kernalRomSignatureCnt][3] = {

    { 0x85, 0x56, 0x20 },       // Commodore ROM
    { 0x20, 0x70, 0xA8 },       // MEGA65 project
    { 0x20, 0x02, 0xBE }
};

const u8 C64RomFile::magicVC1541RomBytes[vc1541RomSignatureCnt][3] = {

    { 0x97, 0xAA, 0xAA },       // Commodore ROM
    { 0x97, 0xE0, 0x43 },       // Commodore ROM
    { 0x97, 0x46, 0xAD },       // Commodore ROM
    { 0x97, 0xDB, 0x43 }        // Commodore ROM
};

bool
C64RomFile::isRomBuffer(const u8 *buffer, size_t length)
{
    return
    isBasicRomBuffer(buffer, length) ||
    isCharRomBuffer(buffer, length) ||
    isKernalRomBuffer(buffer, length) ||
    isVC1541RomBuffer(buffer, length);
}

bool
C64RomFile::isBasicRomBuffer(const u8 *buffer, size_t length)
{
    if (length != 0x2000) return false;

    for (int i = 0; i < basicRomSignatureCnt; i++) {
        if (matchingBufferHeader(buffer, magicBasicRomBytes[i], sizeof(magicBasicRomBytes[i]))) {
            return true;
        }
    }

    return false;
}

bool
C64RomFile::isCharRomBuffer(const u8 *buffer, size_t length)
{
    if (length != 0x1000) return false;

    for (int i = 0; i < charRomSignatureCnt; i++) {
        if (matchingBufferHeader(buffer, magicCharRomBytes[i], sizeof(magicCharRomBytes[i]))) {
            return true;
        }
    }

    return false;
}

bool
C64RomFile::isKernalRomBuffer(const u8 *buffer, size_t length)
{
    if (length != 0x2000) return false;

    for (int i = 0; i < kernalRomSignatureCnt; i++) {
        if (matchingBufferHeader(buffer, magicKernalRomBytes[i], sizeof(magicKernalRomBytes[i]))) {
            return true;
        }
    }

    return false;
}

bool
C64RomFile::isVC1541RomBuffer(const u8 *buffer, size_t length)
{
    if (length != 0x4000) return false;

    for (int i = 0; i < vc1541RomSignatureCnt; i++) {
        if (matchingBufferHeader(buffer, magicVC1541RomBytes[i], sizeof(magicVC1541RomBytes[i]))) {
            return true;
        }
    }

    return false;
}

RomRevision
C64RomFile::revision(u64 fnv)
{
    switch(fnv) {
            
        case 0x0000000000000000: return ROM_MISSING;
            
        case 0x20765FEA67A8762D: return BASIC_COMMODORE;
        case 0xF5E925DCABE2D3F0: return BASIC_MEGA65;
                        
        case 0xACC576F7B332AC15: return CHAR_COMMODORE;
        case 0x3CA9D37AA3DE0969: return CHAR_SWEDISH_C2D007;
        case 0x6E3827A65FFF116F: return CHAR_SWEDISH;
        case 0x623CDF045B74B691: return CHAR_SPANISH_C64C;
        case 0x79A236A3B3645231: return CHAR_DANISH;
        case 0x1130C1CE287876DD: return CHAR_ATARI800;
        case 0x975546A5B6168FFD: return CHAR_MSX;
        case 0x7C74107C9365F735: return CHAR_ZX_SPECTRUM;
        case 0xAFFE8B0EE2176CBD: return CHAR_AMSTRAD_CPC;
        case 0xD14C5BE4FEE17705: return CHAR_A500_TOPAZ_BROKEN;
        case 0xA2C6A6E2C0477981: return CHAR_A500_TOPAZ_V2;
        case 0x3BF55C821EE80365: return CHAR_A1200_TOPAZ_BROKEN;
        case 0x19F0DD3F3F9C4FE9: return CHAR_A1200_TOPAZ_V2;
        case 0xE527AD3E0DDE930D: return CHAR_TELETEXT;
        case 0x4D31ECBF4F967DC3: return CHAR_MEGA65_GENERIC;
            
        case 0xFB166E49AF709AB8: return KERNAL_1ST;
        case 0x4232D81CCD24FAAE: return KERNAL_2ST;
        case 0x4AF60EE54BEC9701: return KERNAL_3ST;
        case 0x429EA22675CAB478: return KERNAL_DANISH_3RD;
        case 0x8C4548E2202CB366: return KERNAL_SX64;
        case 0x746EB1BC008B07E1: return KERNAL_SX64_SCAND;
        case 0xDE7F07008B787040: return KERNAL_64_JIFFY;
        case 0xA9D2AD1A4E5F782C: return KERNAL_SX64_JIFFY;
        case 0x750617B8DE6DBA82: return KERNAL_TURBO_ROM;
        case 0x7E0A124C3F192818: return KERNAL_DATEL_V32;
        case 0x211EAC45AB03A2CA: return KERNAL_EXOS_V3;
        case 0xF2A39FF166D338AE: return KERNAL_TURBO_TAPE;
        case 0xCB54D7934B99ADAC: return KERNAL_MEGA65_GENERIC;
            
        case 0x44BBA0EAC5898597: return VC1541_II_1987;
        case 0xA1D36980A17C8756: return VC1541_II_NEWTRONIC;
        case 0x361A1EC48F04F5A4: return VC1541_OLD_WHITE;
        case 0xB938E2DA07F4FE40: return VC1541_1541C;
        case 0x47CBA55F16FB3E09: return VC1541_II_RELOC_PATCH;
        case 0x8B2A523E29BED889: return VC1541_II_JIFFY;
        case 0xF7F4D931219DBB5D: return VC1541_II_JIFFY_V600;
            
        default: return ROM_UNKNOWN;
    }
}

const char *
C64RomFile::title(RomRevision rev)
{
    switch (rev) {
            
        case ROM_UNKNOWN:             return "Unknown or patched Rom";
            
        case BASIC_COMMODORE:         return "Commodore 64 Basic V2";
        case BASIC_MEGA65:            return "M.E.G.A. C64 OpenROM";
            
        case CHAR_COMMODORE:
        case CHAR_SWEDISH_C2D007:
        case CHAR_SWEDISH:
        case CHAR_SPANISH_C64C:
        case CHAR_DANISH:             return "Commodore Character Set";
        case CHAR_ATARI800:           return "Atari 800 character set";
        case CHAR_MSX:                return "MSX Character Set";
        case CHAR_ZX_SPECTRUM:        return "ZX Spectrum Character Set";
        case CHAR_AMSTRAD_CPC:        return "Amstrad CPC Character Set";
        case CHAR_A500_TOPAZ_BROKEN:
        case CHAR_A500_TOPAZ_V2:      return "Amiga 500 Character Set";
        case CHAR_A1200_TOPAZ_BROKEN:
        case CHAR_A1200_TOPAZ_V2:     return "Amiga 1200 Character Set";
        case CHAR_TELETEXT:           return "Teletext Character Set";
        case CHAR_MEGA65_GENERIC:     return "M.E.G.A. C64 OpenROM";
            
        case KERNAL_1ST:
        case KERNAL_2ST:
        case KERNAL_3ST:              return "Commodore 64 Kernal";
        case KERNAL_DANISH_3RD:       return "Commodore 64 Kernal (Danish)";
        case KERNAL_SX64:             return "Commodore SX-64 Kernal";
        case KERNAL_SX64_SCAND:       return "SX-64 Kernal (Scandinavian)";
        case KERNAL_64_JIFFY:         return "C64 Kernel (JiffyDOS patch)";
        case KERNAL_SX64_JIFFY:       return "SX-64 Kernal (JiffyDOS patch)";
        case KERNAL_TURBO_ROM:        return "Cockroach Turbo-ROM";
        case KERNAL_DATEL_V32:        return "Datel Turbo Rom II";
        case KERNAL_EXOS_V3:          return "Exos Kernal ROM";
        case KERNAL_TURBO_TAPE:       return "Turbo Tape ROM";
        case KERNAL_MEGA65_GENERIC:   return "M.E.G.A. C64 OpenROM";
            
        case VC1541_II_1987:
        case VC1541_II_NEWTRONIC:     return "VC1541-II firmware";
        case VC1541_OLD_WHITE:        return "VC1541 firmware";
        case VC1541_1541C:            return "VC1541C firmware";
        case VC1541_II_RELOC_PATCH:
        case VC1541_II_JIFFY:
        case VC1541_II_JIFFY_V600:    return "VC1541-II firmware";
            
        default:                      return "";
    }
}

const char *
C64RomFile::version(RomRevision rev)
{
    switch (rev) {
            
        case BASIC_COMMODORE:         return "Version 2";
        case BASIC_MEGA65:            return "";
            
        case CHAR_COMMODORE:
        case CHAR_SWEDISH_C2D007:     return "Swedish C64 (C2D007)";
        case CHAR_SWEDISH:            return "Spanish C64";
        case CHAR_SPANISH_C64C:       return "Spanish C64C";
        case CHAR_DANISH:             return "Danish C64";
        case CHAR_ATARI800:
        case CHAR_MSX:
        case CHAR_ZX_SPECTRUM:
        case CHAR_AMSTRAD_CPC:
        case CHAR_A500_TOPAZ_BROKEN:  return "Topaz v1 (broken)";
        case CHAR_A500_TOPAZ_V2:      return "Topaz v2";
        case CHAR_A1200_TOPAZ_BROKEN: return "Topaz v1 (broken)";
        case CHAR_A1200_TOPAZ_V2:     return "Topaz v2";
        case CHAR_TELETEXT:
        case CHAR_MEGA65_GENERIC:     return "";
            
        case KERNAL_1ST:              return "Rev. 1";
        case KERNAL_2ST:              return "Rev. 2";
        case KERNAL_3ST:
        case KERNAL_DANISH_3RD:       return "Rev. 3";
        case KERNAL_SX64:
        case KERNAL_SX64_SCAND:
        case KERNAL_64_JIFFY:
        case KERNAL_SX64_JIFFY:       return "";
        case KERNAL_TURBO_ROM:        return "v1";
        case KERNAL_DATEL_V32:        return "v3.2+";
        case KERNAL_EXOS_V3:          return "v3";
        case KERNAL_TURBO_TAPE:       return "v0.1";
        case KERNAL_MEGA65_GENERIC:   return "";
            
        case VC1541_II_1987:          return "";
        case VC1541_II_NEWTRONIC:     return "Drives with Newtronic motor";
        case VC1541_OLD_WHITE:        return "Drives with white chassis";
        case VC1541_1541C:            return "";
        case VC1541_II_RELOC_PATCH:   return "Reloc patch";
        case VC1541_II_JIFFY:         return "JiffyDOS patch";
        case VC1541_II_JIFFY_V600:    return "JiffyDOS patch v6.0";
            
        default:                      return "";
    }
}

const char *
C64RomFile::released(RomRevision rev)
{
    switch (rev) {
            
        case BASIC_COMMODORE:
        case BASIC_MEGA65:            return "";
            
        case CHAR_COMMODORE:
        case CHAR_SWEDISH_C2D007:
        case CHAR_SWEDISH:
        case CHAR_SPANISH_C64C:
        case CHAR_DANISH:
        case CHAR_ATARI800:
        case CHAR_MSX:
        case CHAR_ZX_SPECTRUM:
        case CHAR_AMSTRAD_CPC:
        case CHAR_A500_TOPAZ_BROKEN:
        case CHAR_A500_TOPAZ_V2:
        case CHAR_A1200_TOPAZ_BROKEN:
        case CHAR_A1200_TOPAZ_V2:
        case CHAR_TELETEXT:
        case CHAR_MEGA65_GENERIC:     return "";
            
        case KERNAL_1ST:
        case KERNAL_2ST:              return "1982";
        case KERNAL_3ST:
        case KERNAL_DANISH_3RD:
        case KERNAL_SX64:
        case KERNAL_SX64_SCAND:
        case KERNAL_64_JIFFY:
        case KERNAL_SX64_JIFFY:
        case KERNAL_TURBO_ROM:
        case KERNAL_DATEL_V32:
        case KERNAL_EXOS_V3:
        case KERNAL_TURBO_TAPE:
        case KERNAL_MEGA65_GENERIC:
            
        case VC1541_II_1987:          return "1987";
        case VC1541_II_NEWTRONIC:
        case VC1541_OLD_WHITE:
        case VC1541_1541C:
        case VC1541_II_RELOC_PATCH:
        case VC1541_II_JIFFY:
        case VC1541_II_JIFFY_V600:    return "";
            
        default:                      return "";
    }
}

bool
C64RomFile::isRomFile(const char *filename)
{
    return
    isBasicRomFile(filename) ||
    isCharRomFile(filename) ||
    isKernalRomFile(filename) ||
    isVC1541RomFile(filename);
}

bool
C64RomFile::isBasicRomFile(const char *filename)
{
    if (!checkFileSize(filename, 0x2000, 0x2000)) return false;

    for (int i = 0; i < basicRomSignatureCnt; i++) {
        if (matchingFileHeader(filename, magicBasicRomBytes[i], sizeof(magicBasicRomBytes[i]))) {
            return true;
        }
    }

    return false;
}

bool
C64RomFile::isCharRomFile(const char *filename)
{
    if (!checkFileSize(filename, 0x1000, 0x1000)) return false;

    for (int i = 0; i < charRomSignatureCnt; i++) {
        if (matchingFileHeader(filename, magicCharRomBytes[i], sizeof(magicCharRomBytes[i]))) {
            return true;
        }
    }

    return false;
}

bool
C64RomFile::isKernalRomFile(const char *filename)
{
    if (!checkFileSize(filename, 0x2000, 0x2000)) return false;

    for (int i = 0; i < kernalRomSignatureCnt; i++) {
        if (matchingFileHeader(filename, magicKernalRomBytes[i], sizeof(magicKernalRomBytes[i]))) {
            return true;
        }
    }

    return false;
}

bool
C64RomFile::isVC1541RomFile(const char *filename)
{
    if (!checkFileSize(filename, 0x4000, 0x4000)) return false;

    for (int i = 0; i < vc1541RomSignatureCnt; i++) {
        if (matchingFileHeader(filename, magicVC1541RomBytes[i], sizeof(magicVC1541RomBytes[i]))) {
            return true;
        }
    }

    return false;
}

C64RomFile *
C64RomFile::makeWithBuffer(const u8 *buffer, size_t length)
{
    C64RomFile *rom = new C64RomFile();
    
    if (!rom->readFromBuffer(buffer, length)) {
        delete rom;
        return NULL;
    }
    
    return rom;
}

C64RomFile *
C64RomFile::makeWithFile(const char *filename)
{
    C64RomFile *rom = new C64RomFile();
    
    if (!rom->readFromFile(filename)) {
        delete rom;
        return NULL;
    }
    
    return rom;
}

C64RomFile::C64RomFile()
{
    setDescription("RomFile");
    romtype = UNKNOWN_FILE_FORMAT;
}

bool
C64RomFile::readFromBuffer(const u8 *buffer, size_t length)
{
    if (!AnyC64File::readFromBuffer(buffer, length))
        return false;
    
    romtype =
    isBasicRomBuffer(buffer, length) ? BASIC_ROM_FILE :
    isCharRomBuffer(buffer, length) ? CHAR_ROM_FILE :
    isKernalRomBuffer(buffer, length) ? KERNAL_ROM_FILE :
    isVC1541RomBuffer(buffer, length) ? VC1541_ROM_FILE :
    UNKNOWN_FILE_FORMAT;
 
    return romtype != UNKNOWN_FILE_FORMAT;
}
