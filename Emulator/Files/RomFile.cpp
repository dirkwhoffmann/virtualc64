// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "RomFile.h"
#include "C64.h"
#include "IO.h"

const RomSignature RomFile::signatures[] = {
   
    { ROM_TYPE_BASIC,  0x2000, 0x0000, { 0x94, 0xE3, 0x7B } }, // Commodore
    { ROM_TYPE_BASIC,  0x2000, 0x0000, { 0x63, 0xA6, 0xC1 } }, // MEGA65
    { ROM_TYPE_BASIC,  0x2000, 0x0000, { 0x94, 0xE3, 0xB7 } }, // MEGA65
    
    { ROM_TYPE_CHAR,   0x1000, 0x0000, { 0x3C, 0x66, 0x6E } }, // Commodore
    { ROM_TYPE_CHAR,   0x1000, 0x0000, { 0x00, 0x3C, 0x66 } }, // Atari800
    { ROM_TYPE_CHAR,   0x1000, 0x0000, { 0x70, 0x88, 0x08 } }, // MSX
    { ROM_TYPE_CHAR,   0x1000, 0x0000, { 0x00, 0x3C, 0x4A } }, // Speccy
    { ROM_TYPE_CHAR,   0x1000, 0x0000, { 0x7C, 0xC6, 0xDE } }, // Amstrad, Amiga
    { ROM_TYPE_CHAR,   0x1000, 0x0000, { 0x38, 0x44, 0x5C } }, // Speccy

    { ROM_TYPE_KERNAL, 0x2000, 0x0000, { 0x85, 0x56, 0x20 } }, // Commodore
    { ROM_TYPE_KERNAL, 0x2000, 0x0000, { 0xA9, 0x93, 0x20 } }, // MEGA65
    { ROM_TYPE_KERNAL, 0x2000, 0x0000, { 0x20, 0x2E, 0xBA } }, // MEGA65
    { ROM_TYPE_KERNAL, 0x2000, 0x0000, { 0x20, 0x02, 0xBE } }, // MEGA65

    { ROM_TYPE_VC1541, 0x4000, 0x0000, { 0x97, 0xAA, 0xAA } }, // Commodore
    { ROM_TYPE_VC1541, 0x4000, 0x0000, { 0x97, 0xE0, 0x43 } }, // Commodore
    { ROM_TYPE_VC1541, 0x4000, 0x0000, { 0x97, 0x46, 0xAD } }, // Commodore
    { ROM_TYPE_VC1541, 0x4000, 0x0000, { 0x97, 0xDB, 0x43 } }, // Commodore
    { ROM_TYPE_VC1541, 0x6000, 0x0000, { 0x4C, 0x4B, 0xA3 } }, // Dolphin
    { ROM_TYPE_VC1541, 0x8000, 0x2000, { 0x4C, 0x4B, 0xA3 } }, // Dolphin
    
    { ROM_TYPE_COUNT,  0x0000, 0x0000, { 0x00, 0x00, 0x00 } }
};

bool
RomFile::isCompatibleName(const std::string &name)
{
    return true;
}

bool
RomFile::isCompatibleStream(std::istream &stream)
{
    return
    isBasicRomStream(stream) ||
    isCharRomStream(stream) ||
    isKernalRomStream(stream) ||
    isVC1541RomStream(stream);
}

bool
RomFile::isRomStream(RomType type, std::istream &is)
{
    isize size = util::streamLength(is);
    
    for (isize i = 0; signatures[i].type != ROM_TYPE_COUNT; i++) {
            
        // Only proceed if the file type matches
        if (signatures[i].type != type) continue;
        
        // Only proceed if the file size matches
        if (signatures[i].size != size) continue;

        // Only proceed if the matches bytes matche
        if (!util::matchingStreamHeader(is, signatures[i].magic, 3, signatures[i].offset))
            continue;

        return true;
    }
        
    return false;
}

bool
RomFile::isBasicRomStream(std::istream &is)
{
    return isRomStream(ROM_TYPE_BASIC, is);
}

bool
RomFile::isCharRomStream(std::istream &is)
{
    return isRomStream(ROM_TYPE_CHAR, is);
}

bool
RomFile::isKernalRomStream(std::istream &is)
{
    return isRomStream(ROM_TYPE_KERNAL, is);
}

bool
RomFile::isVC1541RomStream(std::istream &is)
{
    return isRomStream(ROM_TYPE_VC1541, is);
}

bool
RomFile::isRomFile(RomType type, const char *path)
{
    std::ifstream stream(path);
    return isRomStream(type, stream);
}

bool
RomFile::isBasicRomFile(const char *path)
{
    return isRomFile(ROM_TYPE_BASIC, path);
}

bool
RomFile::isCharRomFile(const char *path)
{
    return isRomFile(ROM_TYPE_CHAR, path);
}

bool
RomFile::isKernalRomFile(const char *path)
{
    return isRomFile(ROM_TYPE_KERNAL, path);
}

bool
RomFile::isVC1541RomFile(const char *path)
{
    return isRomFile(ROM_TYPE_VC1541, path);
}

bool
RomFile::isRomBuffer(RomType type, const u8 *buf, usize len)
{
    std::stringstream stream;
    stream.write((const char *)buf, len);
    return isRomStream(type, stream);
}

bool
RomFile::isBasicRomBuffer(const u8 *buf, usize len)
{
    return isRomBuffer(ROM_TYPE_BASIC, buf, len);
}

bool
RomFile::isCharRomBuffer(const u8 *buf, usize len)
{
    return isRomBuffer(ROM_TYPE_CHAR, buf, len);
}

bool
RomFile::isKernalRomBuffer(const u8 *buf, usize len)
{
    return isRomBuffer(ROM_TYPE_KERNAL, buf, len);
}

bool
RomFile::isVC1541RomBuffer(const u8 *buf, usize len)
{
    return isRomBuffer(ROM_TYPE_VC1541, buf, len);
}

RomIdentifier
RomFile::identifier(u64 fnv)
{
    switch(fnv) {
            
        case 0x0000000000000000: return ROM_MISSING;
            
        case 0x20765FEA67A8762D: return BASIC_COMMODORE;
                        
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
        case 0x4D31ECBF4F967DC3: return CHAR_MEGA65;
        case 0x564e103e962682dd: return CHAR_PXLFONT_V23;
            
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
        case 0x7202DEA530E1C172: return KERNAL_64ER_V3;
        case 0x7E3AEFF7886684A2: return KERNAL_SPEEDDOS_PLUS;
        case 0x6E6190177D93D2BB: return KERNAL_SPEEDDOS_27;
        case 0xD135F409F4FA10F2: return KERNAL_DOLPHIN_10;
        case 0x32CAF94AAA196DB6: return KERNAL_DOLPHIN_20_1;
        case 0xAE3DEC803423CE60: return KERNAL_DOLPHIN_20_1_MA;
        case 0x4D3C32F9415972C3: return KERNAL_DOLPHIN_20_2;
        case 0xA5D930343EE32459: return KERNAL_DOLPHIN_20_3;
        case 0x7D34E8277F74A321: return KERNAL_DOLPHIN_20_SLVDR;
        case 0x877E38DA5DAFEC30: return KERNAL_DOLPHIN_30;
            
        case 0x44BBA0EAC5898597: return VC1541_II_1987;
        case 0xA1D36980A17C8756: return VC1541_II_NEWTRONIC;
        case 0x361A1EC48F04F5A4: return VC1541_OLD_WHITE;
        case 0xB938E2DA07F4FE40: return VC1541_1541C;
        case 0x47CBA55F16FB3E09: return VC1541_II_RELOC_PATCH;
        case 0x8B2A523E29BED889: return VC1541_II_JIFFY;
        case 0xF7F4D931219DBB5D: return VC1541_II_JIFFY_V600;
        case 0xB4027D6D9D61378A: return VC1541_64ER_V3;
        case 0xC50EAFCBA50C4B63: return VC1541_SPEEDDOS_PLUS;
        case 0x92ADEBA1BCCD8D31: return VC1541_SPEEDDOS_27;
        case 0x28CD4E47A40C41CA: return VC1541_DOLPHIN_20;
        case 0x1C1DDD64E02CAD32: return VC1541_DOLPHIN_20_SLVDR;
        case 0x09D8FBAB61E59FF0: return VC1541_DOLPHIN_30;

        default: return ROM_UNKNOWN;
    }
}

bool
RomFile::isCommodoreRom(RomIdentifier rev)
{
    switch (rev) {
            
        case BASIC_COMMODORE:
        case CHAR_COMMODORE:
        case CHAR_SWEDISH_C2D007:
        case CHAR_SWEDISH:
        case CHAR_SPANISH_C64C:
        case CHAR_DANISH:
        case KERNAL_1ST:
        case KERNAL_2ST:
        case KERNAL_3ST:
        case KERNAL_DANISH_3RD:
        case KERNAL_SX64:
        case KERNAL_SX64_SCAND:
        case VC1541_II_1987:
        case VC1541_II_NEWTRONIC:
        case VC1541_OLD_WHITE:
        case VC1541_1541C:
            return true;
            
        default:
            return false;
    }
}

bool
RomFile::isPatchedRom(RomIdentifier rev)
{
    switch (rev) {
            
        case CHAR_ATARI800:
        case CHAR_MSX:
        case CHAR_ZX_SPECTRUM:
        case CHAR_AMSTRAD_CPC:
        case CHAR_A500_TOPAZ_BROKEN:
        case CHAR_A500_TOPAZ_V2:
        case CHAR_A1200_TOPAZ_BROKEN:
        case CHAR_A1200_TOPAZ_V2:
        case CHAR_TELETEXT:
            
        case KERNAL_64_JIFFY:
        case KERNAL_SX64_JIFFY:
        case KERNAL_TURBO_ROM:
        case KERNAL_DATEL_V32:
        case KERNAL_EXOS_V3:
        case KERNAL_TURBO_TAPE:
        case KERNAL_64ER_V3:
        case KERNAL_SPEEDDOS_PLUS:
        case KERNAL_SPEEDDOS_27:
        case KERNAL_DOLPHIN_10:
        case KERNAL_DOLPHIN_20_1:
        case KERNAL_DOLPHIN_20_1_MA:
        case KERNAL_DOLPHIN_20_2:
        case KERNAL_DOLPHIN_20_3:
        case KERNAL_DOLPHIN_20_SLVDR:
        case KERNAL_DOLPHIN_30:
            
        case VC1541_II_RELOC_PATCH:
        case VC1541_II_JIFFY:
        case VC1541_II_JIFFY_V600:
        case VC1541_64ER_V3:
        case VC1541_SPEEDDOS_PLUS:
        case VC1541_SPEEDDOS_27:
        case VC1541_DOLPHIN_20:
        case VC1541_DOLPHIN_20_SLVDR:
        case VC1541_DOLPHIN_30:
            return true;
            
        default:
            return false;
    }
}

const char *
RomFile::title(RomIdentifier rev)
{
    switch (rev) {
            
        case ROM_UNKNOWN:             return "Unknown";
            
        case BASIC_COMMODORE:         return "Basic Rom";
            
        case CHAR_COMMODORE:
        case CHAR_SWEDISH_C2D007:
        case CHAR_SWEDISH:
        case CHAR_SPANISH_C64C:
        case CHAR_DANISH:             return "Character Rom";
        case CHAR_ATARI800:
        case CHAR_MSX:
        case CHAR_ZX_SPECTRUM:
        case CHAR_AMSTRAD_CPC:
        case CHAR_A500_TOPAZ_BROKEN:
        case CHAR_A500_TOPAZ_V2:
        case CHAR_A1200_TOPAZ_BROKEN:
        case CHAR_A1200_TOPAZ_V2:
        case CHAR_TELETEXT:           return "Patched Character Rom";
        case CHAR_MEGA65:             return "M.E.G.A. C64 OpenROM";
        case CHAR_PXLFONT_V23:        return "M.E.G.A. C64 OpenROM";
            
        case KERNAL_1ST:
        case KERNAL_2ST:
        case KERNAL_3ST:
        case KERNAL_DANISH_3RD:
        case KERNAL_SX64:
        case KERNAL_SX64_SCAND:       return "Kernal Rom";
        case KERNAL_64_JIFFY:
        case KERNAL_SX64_JIFFY:
        case KERNAL_TURBO_ROM:
        case KERNAL_DATEL_V32:
        case KERNAL_EXOS_V3:
        case KERNAL_TURBO_TAPE:
        case KERNAL_64ER_V3:
        case KERNAL_SPEEDDOS_PLUS:
        case KERNAL_SPEEDDOS_27:
        case KERNAL_DOLPHIN_10:
        case KERNAL_DOLPHIN_20_1:
        case KERNAL_DOLPHIN_20_1_MA:
        case KERNAL_DOLPHIN_20_2:
        case KERNAL_DOLPHIN_20_3:
        case KERNAL_DOLPHIN_20_SLVDR:
        case KERNAL_DOLPHIN_30:       return "Patched Kernal Rom";
            
        case VC1541_II_1987:        
        case VC1541_II_NEWTRONIC:
        case VC1541_OLD_WHITE:
        case VC1541_1541C:            return "Floppy Drive Firmware";
        case VC1541_II_RELOC_PATCH:
        case VC1541_II_JIFFY:
        case VC1541_II_JIFFY_V600:
        case VC1541_64ER_V3:
        case VC1541_SPEEDDOS_PLUS:
        case VC1541_SPEEDDOS_27:
        case VC1541_DOLPHIN_20:
        case VC1541_DOLPHIN_20_SLVDR:
        case VC1541_DOLPHIN_30:       return "Patched Drive Firmware";
            
        default:                      return "";
    }
}

const char *
RomFile::subTitle(RomIdentifier rev)
{
    switch (rev) {
            
        case BASIC_COMMODORE:         return "Generic C64";
            
        case CHAR_COMMODORE:          return "Generic C64";
        case CHAR_SWEDISH_C2D007:     return "Swedish C64 (C2D007)";
        case CHAR_SWEDISH:            return "Spanish C64";
        case CHAR_SPANISH_C64C:       return "Spanish C64C";
        case CHAR_DANISH:             return "Danish C64";
        case CHAR_ATARI800:           return "Atari 800 Font";
        case CHAR_MSX:                return "MSX Font";
        case CHAR_ZX_SPECTRUM:        return "ZX Spectrum Font";
        case CHAR_AMSTRAD_CPC:        return "Amstrad Font";
        case CHAR_A500_TOPAZ_BROKEN:  return "Topaz Font";
        case CHAR_A500_TOPAZ_V2:      return "Topaz Font";
        case CHAR_A1200_TOPAZ_BROKEN: return "Topaz Font";
        case CHAR_A1200_TOPAZ_V2:     return "Topaz Font";
        case CHAR_TELETEXT:           return "Teletext Font";
        case CHAR_MEGA65:             return "M.E.G.A. C64 OpenROM";
        case CHAR_PXLFONT_V23:        return "PXLfont";

        case KERNAL_1ST:              return "Generic C64";
        case KERNAL_2ST:              return "Generic C64";
        case KERNAL_3ST:              return "Generic C64";
        case KERNAL_DANISH_3RD:       return "Danish C64";
        case KERNAL_SX64:             return "Generic SX64";
        case KERNAL_SX64_SCAND:       return "Scandinavian SX64";
        case KERNAL_64_JIFFY:         return "JiffyDOS";
        case KERNAL_SX64_JIFFY:       return "JiffyDOS (SX64)";
        case KERNAL_TURBO_ROM:        return "Turbo Rom";
        case KERNAL_DATEL_V32:        return "Datel Rom";
        case KERNAL_EXOS_V3:          return "Exos Rom";
        case KERNAL_TURBO_TAPE:       return "Turbo Tape";
        case KERNAL_64ER_V3:          return "64'er DOS V3";
        case KERNAL_SPEEDDOS_PLUS:    return "SpeedDOS Plus";
        case KERNAL_SPEEDDOS_27:      return "SpeedDOS Plus (TRIAD)";
        case KERNAL_DOLPHIN_10:       return "Dolphin DOS";
        case KERNAL_DOLPHIN_20_1:     return "Dolphin DOS";
        case KERNAL_DOLPHIN_20_1_MA:  return "Dolphin DOS";
        case KERNAL_DOLPHIN_20_2:     return "Dolphin DOS";
        case KERNAL_DOLPHIN_20_3:     return "Dolphin DOS";
        case KERNAL_DOLPHIN_20_SLVDR: return "Dolphin DOS";
        case KERNAL_DOLPHIN_30:       return "Dolphin DOS";
            
        case VC1541_II_1987:          return "VC1541-II (1987)";
        case VC1541_II_NEWTRONIC:     return "VC1541-II (Newtronic)";
        case VC1541_OLD_WHITE:        return "VC1541 (White)";
        case VC1541_1541C:            return "VC1541C";
        case VC1541_II_RELOC_PATCH:   return "Relocation Patch";
        case VC1541_II_JIFFY:         return "JiffyDOS Patch";
        case VC1541_II_JIFFY_V600:    return "JiffyDOS Patch";
        case VC1541_64ER_V3:          return "64'er DOS";
        case VC1541_SPEEDDOS_PLUS:    return "SpeedDOS Plus";
        case VC1541_SPEEDDOS_27:      return "SpeedDOS Plus";
        case VC1541_DOLPHIN_20:       return "Dolphin DOS";
        case VC1541_DOLPHIN_20_SLVDR: return "Dolphin DOS";
        case VC1541_DOLPHIN_30:       return "Dolphin DOS";
            
        default:                      return "";
    }
}

const char *
RomFile::revision(RomIdentifier rev)
{
    switch (rev) {
            
        case BASIC_COMMODORE:         return "V2";
            
        case CHAR_COMMODORE:          return "V1";
        case CHAR_SWEDISH_C2D007:     return "";
        case CHAR_SWEDISH:            return "";
        case CHAR_SPANISH_C64C:       return "";
        case CHAR_DANISH:             return "V3";
        case CHAR_ATARI800:           return "";
        case CHAR_MSX:                return "";
        case CHAR_ZX_SPECTRUM:        return "";
        case CHAR_AMSTRAD_CPC:        return "";
        case CHAR_A500_TOPAZ_BROKEN:  return "";
        case CHAR_A500_TOPAZ_V2:      return "V2";
        case CHAR_A1200_TOPAZ_BROKEN: return "V2 (broken)";
        case CHAR_A1200_TOPAZ_V2:     return "V2";
        case CHAR_TELETEXT:           return "";
        case CHAR_MEGA65:             return "";
        case CHAR_PXLFONT_V23:        return "V2.3";

        case KERNAL_1ST:              return "V1";
        case KERNAL_2ST:              return "V2";
        case KERNAL_3ST:              return "V3";
        case KERNAL_DANISH_3RD:       return "V3";
        case KERNAL_SX64:             return "";
        case KERNAL_SX64_SCAND:       return "";
        case KERNAL_64_JIFFY:         return "";
        case KERNAL_SX64_JIFFY:       return "";
        case KERNAL_TURBO_ROM:        return "V1";
        case KERNAL_DATEL_V32:        return "V3.2+";
        case KERNAL_EXOS_V3:          return "V3";
        case KERNAL_TURBO_TAPE:       return "V0.1";
        case KERNAL_64ER_V3:          return "V3";
        case KERNAL_SPEEDDOS_PLUS:    return "";
        case KERNAL_SPEEDDOS_27:      return "V2.7 (TRIAD)";
        case KERNAL_DOLPHIN_10:       return "V1.0";
        case KERNAL_DOLPHIN_20_1:     return "V2.0 Rev 1";
        case KERNAL_DOLPHIN_20_1_MA:  return "V2.0 Rev 1 (M.A.)";
        case KERNAL_DOLPHIN_20_2:     return "V2.0 Rev 2";
        case KERNAL_DOLPHIN_20_3:     return "V2.0 Rev 3";
        case KERNAL_DOLPHIN_20_SLVDR: return "V2.0 (SilverDream)";
        case KERNAL_DOLPHIN_30:       return "V3.0";

            
        case VC1541_II_1987:          return "";
        case VC1541_II_NEWTRONIC:     return "";
        case VC1541_OLD_WHITE:        return "";
        case VC1541_1541C:            return "";
        case VC1541_II_RELOC_PATCH:   return "";
        case VC1541_II_JIFFY:         return "";
        case VC1541_II_JIFFY_V600:    return "V6.0";
        case VC1541_64ER_V3:          return "V3";
        case VC1541_SPEEDDOS_PLUS:    return "";
        case VC1541_SPEEDDOS_27:      return "V2.7 (TRIAD)";
        case VC1541_DOLPHIN_20:       return "V2.0";
        case VC1541_DOLPHIN_20_SLVDR: return "V2.0 (SilverDream)";
        case VC1541_DOLPHIN_30:       return "V3.0";

        default:                      return "";
    }
}

isize
RomFile::readFromStream(std::istream &stream)
{
    romFileType =
    isBasicRomStream(stream) ? FILETYPE_BASIC_ROM :
    isCharRomStream(stream) ? FILETYPE_CHAR_ROM :
    isKernalRomStream(stream) ? FILETYPE_KERNAL_ROM :
    isVC1541RomStream(stream) ? FILETYPE_VC1541_ROM :
    FILETYPE_UNKNOWN;
    
    return AnyFile::readFromStream(stream);
}

void
RomFile::repair()
{
    // Count the number of 0xFF bytes at the beginning of the file
    isize pads = 0; for (; data[pads] == 0xFF; pads++);
    
    // Align to a 1KB grid
    pads &= ~0x3FF;
    
    // Remove the padding bytes
    if (pads) {
        msg("Removing %zx padding bytes from Rom file\n", pads);
        strip(pads & ~0x3FF);
    }
}
