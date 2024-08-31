// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// This FILE is dual-licensed. You are free to choose between:
//
//     - The GNU General Public License v3 (or any later version)
//     - The Mozilla Public License v2
//
// SPDX-License-Identifier: GPL-3.0-or-later OR MPL-2.0
// -----------------------------------------------------------------------------

#include "config.h"
#include "RomFile.h"
#include "C64.h"
#include "IOUtils.h"

namespace vc64 {

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
    
    { RomType(0),      0x0000, 0x0000, { 0x00, 0x00, 0x00 } }
};

bool
RomFile::isCompatible(const fs::path &path)
{
    return true;
}

bool
RomFile::isCompatible(const u8 *buf, isize len)
{
    return
    isBasicRomBuffer(buf, len) ||
    isCharRomBuffer(buf, len) ||
    isKernalRomBuffer(buf, len) ||
    isVC1541RomBuffer(buf, len);
}

bool
RomFile::isCompatible(const Buffer<u8> &buf)
{
    return isCompatible(buf.ptr, buf.size);
}

bool
RomFile::isRomBuffer(RomType type, const u8 *buf, isize len)
{
    for (isize i = 0; signatures[i].size != 0; i++) {

        auto magic = signatures[i].magic;
        auto offset = signatures[i].offset;

        // Only proceed if the file type matches
        if (signatures[i].type != type) continue;

        // Only proceed if the file size matches
        if (signatures[i].size != len) continue;

        // Only proceed if the matches bytes matche
        if (!util::matchingBufferHeader(buf, magic, 3, offset)) continue;

        return true;
    }

    return false;
}

bool
RomFile::isBasicRomBuffer(const u8 *buf, isize len)
{
    return isRomBuffer(ROM_TYPE_BASIC, buf, len);
}

bool
RomFile::isCharRomBuffer(const u8 *buf, isize len)
{
    return isRomBuffer(ROM_TYPE_CHAR, buf, len);
}

bool
RomFile::isKernalRomBuffer(const u8 *buf, isize len)
{
    return isRomBuffer(ROM_TYPE_KERNAL, buf, len);
}

bool
RomFile::isVC1541RomBuffer(const u8 *buf, isize len)
{
    return isRomBuffer(ROM_TYPE_VC1541, buf, len);
}

bool
RomFile::isRomBuffer(RomType type, const Buffer<u8> &buf)
{
    return isRomBuffer(type, buf.ptr, buf.size);
}

bool
RomFile::isBasicRomBuffer(const Buffer<u8> &buf)
{
    return isRomBuffer(ROM_TYPE_BASIC, buf.ptr, buf.size);
}

bool
RomFile::isCharRomBuffer(const Buffer<u8> &buf)
{
    return isRomBuffer(ROM_TYPE_CHAR, buf.ptr, buf.size);
}

bool
RomFile::isKernalRomBuffer(const Buffer<u8> &buf)
{
    return isRomBuffer(ROM_TYPE_KERNAL, buf.ptr, buf.size);
}

bool
RomFile::isVC1541RomBuffer(const Buffer<u8> &buf)
{
    return isRomBuffer(ROM_TYPE_VC1541, buf.ptr, buf.size);
}

void
RomFile::finalizeRead()
{
    // Determine the ROM type
    romFileType =
    isBasicRomBuffer(data.ptr, data.size) ? FILETYPE_BASIC_ROM :
    isCharRomBuffer(data.ptr, data.size) ? FILETYPE_CHAR_ROM :
    isKernalRomBuffer(data.ptr, data.size) ? FILETYPE_KERNAL_ROM :
    isVC1541RomBuffer(data.ptr, data.size) ? FILETYPE_VC1541_ROM :
    FILETYPE_UNKNOWN;

    // Count the number of 0xFF bytes at the beginning of the file
    isize pads = 0; for (; data[pads] == 0xFF; pads++);
    
    // Align to a 1KB grid
    pads &= ~0x3FF;
    
    // Remove the padding bytes
    if (pads) {
        msg("Removing %ld padding bytes from Rom file\n", pads);
        strip(pads & ~0x3FF);
    }
}

}
