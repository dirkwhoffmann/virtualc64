/*
 * (C) 2018 Dirk W. Hoffmann. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "C64.h"

const uint8_t ROMFile::magicBasicRomBytes[]   = { 0x94, 0xE3, 0x7B, 0x00 };
const uint8_t ROMFile::magicCharRomBytes[]    = { 0x3C, 0x66, 0x6E, 0x00 };
const uint8_t ROMFile::magicKernalRomBytes[]  = { 0x85, 0x56, 0x20, 0x00 };
const uint8_t ROMFile::magicVC1541RomBytes1[] = { 0x97, 0xAA, 0xAA, 0x00 };
const uint8_t ROMFile::magicVC1541RomBytes2[] = { 0x97, 0xE0, 0x43, 0x00 };

bool
ROMFile::isRom(const uint8_t *buffer, size_t length)
{
    return
    isBasicRom(buffer, length) ||
    isCharRom(buffer, length) ||
    isKernalRom(buffer, length) ||
    isVC1541Rom(buffer, length);
}

bool
ROMFile::isBasicRom(const uint8_t *buffer, size_t length)
{
    if (length != 0x2000) return false;
    return checkBufferHeader(buffer, length, magicBasicRomBytes);
}

bool
ROMFile::isCharRom(const uint8_t *buffer, size_t length)
{
    if (length != 0x1000) return false;
    return checkBufferHeader(buffer, length, magicCharRomBytes);
}

bool
ROMFile::isKernalRom(const uint8_t *buffer, size_t length)
{
    if (length != 0x2000) return false;
    return checkBufferHeader(buffer, length, magicKernalRomBytes);
}

bool
ROMFile::isVC1541Rom(const uint8_t *buffer, size_t length)
{
    if (length != 0x4000) return false;
    return
    checkBufferHeader(buffer, length, magicVC1541RomBytes1) ||
    checkBufferHeader(buffer, length, magicVC1541RomBytes2);
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
    return checkFileHeader(filename, magicBasicRomBytes);
}

bool
ROMFile::isCharRomFile(const char *filename)
{
    if (!checkFileSize(filename, 0x1000, 0x1000)) return false;
    return checkFileHeader(filename, magicCharRomBytes);
}

bool
ROMFile::isKernalRomFile(const char *filename)
{
    if (!checkFileSize(filename, 0x2000, 0x2000)) return false;
    return checkFileHeader(filename, magicKernalRomBytes);
}

bool
ROMFile::isVC1541RomFile(const char *filename)
{
    if (!checkFileSize(filename, 0x4000, 0x4000)) return false;
    return
    checkFileHeader(filename, magicVC1541RomBytes1) ||
    checkFileHeader(filename, magicVC1541RomBytes2);
}

ROMFile *
ROMFile::makeRomFileWithBuffer(const uint8_t *buffer, size_t length)
{
    ROMFile *rom = new ROMFile();
    
    if (!rom->readFromBuffer(buffer, length)) {
        delete rom;
        return NULL;
    }
    
    return rom;
}

ROMFile *
ROMFile::makeRomFileWithFile(const char *filename)
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
    romtype = UNKNOWN_CONTAINER_FORMAT;
}

bool
ROMFile::hasSameType(const char *filename)
{
    return isRomFile(filename);
}

bool
ROMFile::readFromBuffer(const uint8_t *buffer, size_t length)
{
    if (!isRom(buffer, length))
        return false;
    
    if (!AnyC64File::readFromBuffer(buffer, length))
        return false;
    
    romtype =
    isBasicRom(buffer, length) ? BASIC_ROM_FILE :
    isCharRom(buffer, length) ? CHAR_ROM_FILE :
    isKernalRom(buffer, length) ? KERNAL_ROM_FILE :
    VC1541_ROM_FILE;
 
    return true;
}

void
ROMFile::flash(uint8_t *buffer)
{
    memcpy(buffer, data, size);
}

