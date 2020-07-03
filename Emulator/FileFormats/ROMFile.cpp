// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"

const uint8_t ROMFile::magicBasicRomBytes[basicRomSignatureCnt][3] = {

    { 0x94, 0xE3, 0x7B },       // Commodore ROM
    { 0xB9, 0xA7, 0xC1 },       // MEGA65 project
    { 0x63, 0xA6, 0xC1 }        // MEGA65 project

};

const uint8_t ROMFile::magicCharRomBytes[charRomSignatureCnt][4] = {

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

const uint8_t ROMFile::magicKernalRomBytes[kernalRomSignatureCnt][3] = {

    { 0x85, 0x56, 0x20 },       // Commodore ROM
    { 0x20, 0x70, 0xA8 },       // MEGA65 project
    { 0x20, 0x02, 0xBE }
};

const uint8_t ROMFile::magicVC1541RomBytes[vc1541RomSignatureCnt][3] = {

    { 0x97, 0xAA, 0xAA },       // Commodore ROM
    { 0x97, 0xE0, 0x43 },       // Commodore ROM
    { 0x97, 0x46, 0xAD },       // Commodore ROM
    { 0x97, 0xDB, 0x43 }        // Commodore ROM
};

bool
ROMFile::isRomBuffer(const uint8_t *buffer, size_t length)
{
    return
    isBasicRomBuffer(buffer, length) ||
    isCharRomBuffer(buffer, length) ||
    isKernalRomBuffer(buffer, length) ||
    isVC1541RomBuffer(buffer, length);
}

bool
ROMFile::isBasicRomBuffer(const uint8_t *buffer, size_t length)
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
ROMFile::isCharRomBuffer(const uint8_t *buffer, size_t length)
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
ROMFile::isKernalRomBuffer(const uint8_t *buffer, size_t length)
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
ROMFile::isVC1541RomBuffer(const uint8_t *buffer, size_t length)
{
    if (length != 0x4000) return false;

    for (int i = 0; i < vc1541RomSignatureCnt; i++) {
        if (matchingBufferHeader(buffer, magicVC1541RomBytes[i], sizeof(magicVC1541RomBytes[i]))) {
            return true;
        }
    }

    return false;
}

bool
ROMFile::isRomFile(const char *filename)
{
    return
    isBasicRomFile(filename) ||
    isCharRomFile(filename) ||
    isKernalRomFile(filename) ||
    isVC1541RomFile(filename);
}

bool
ROMFile::isBasicRomFile(const char *filename)
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
ROMFile::isCharRomFile(const char *filename)
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
ROMFile::isKernalRomFile(const char *filename)
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
ROMFile::isVC1541RomFile(const char *filename)
{
    if (!checkFileSize(filename, 0x4000, 0x4000)) return false;

    for (int i = 0; i < vc1541RomSignatureCnt; i++) {
        if (matchingFileHeader(filename, magicVC1541RomBytes[i], sizeof(magicVC1541RomBytes[i]))) {
            return true;
        }
    }

    return false;
}

ROMFile *
ROMFile::makeWithBuffer(const uint8_t *buffer, size_t length)
{
    ROMFile *rom = new ROMFile();
    
    if (!rom->readFromBuffer(buffer, length)) {
        delete rom;
        return NULL;
    }
    
    return rom;
}

ROMFile *
ROMFile::makeWithFile(const char *filename)
{
    ROMFile *rom = new ROMFile();
    
    if (!rom->readFromFile(filename)) {
        delete rom;
        return NULL;
    }
    
    return rom;
}

ROMFile::ROMFile()
{
    setDescription("ROMFile");
    romtype = UNKNOWN_FILE_FORMAT;
}

bool
ROMFile::readFromBuffer(const uint8_t *buffer, size_t length)
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
