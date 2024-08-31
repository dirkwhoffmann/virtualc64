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
#include "CRTFile.h"
#include "Cartridge.h"
#include "Checksum.h"
#include "IOUtils.h"

namespace vc64 {

string
CRTFile::cartridgeTypeName(CartridgeType type)
{
    return std::to_string(type) + " (" + CartridgeTypeEnum::key(type) + ")";
}

bool
CRTFile::isCompatible(const fs::path &path)
{
    auto s = util::uppercased(path.extension().string());
    return s == ".CRT";
}

bool
CRTFile::isCompatible(const u8 *buf, isize len)
{
    return util::matchingBufferHeader(buf, len, string("C64 CARTRIDGE   "));
}

bool
CRTFile::isCompatible(const Buffer<u8> &buf)
{
    return isCompatible(buf.ptr, buf.size);
}

PETName<16>
CRTFile::getName() const
{
    return PETName<16>(data.ptr + 0x20, 0x00);
}

void
CRTFile::finalizeRead()
{
    if (CRT_DEBUG) dump();

    // Fix known inconsistencies
    repair();

    // Load chip packets
    u8 *ptr = data.ptr + headerSize();
    for (numberOfChips = 0; ptr < data.ptr + data.size; numberOfChips++) {

        if (numberOfChips == MAX_PACKETS) {
            throw Error(VC64ERROR_CRT_TOO_MANY_PACKETS);
        }
        if (memcmp("CHIP", ptr, 4) != 0) {
            throw Error(VC64ERROR_CRT_CORRUPTED_PACKET);
        }
        
        // Remember start address of each chip section
        chips[numberOfChips] = ptr;
        
        ptr += 0x10;
        ptr += chipSize(numberOfChips);
    }
    
    debug(CRT_DEBUG, "CRT file imported (%ld chips)\n", numberOfChips);
}

CartridgeType
CRTFile::cartridgeType() const {
    
    u16 type = LO_HI(data[0x17], data[0x16]);
    return CartridgeType(type);
}

u16 
CRTFile::cartridgeVersion() const
{
    return LO_HI(data[0x15], data[0x14]);
}

u32
CRTFile::headerSize() const
{
    return HI_HI_LO_LO(data[0x10], data[0x11], data[0x12], data[0x13]);
}

bool
CRTFile::isSupported() const
{
    return Cartridge::isSupportedType(cartridgeType());
}

isize 
CRTFile::chipCount() const 
{
    return numberOfChips;
}

u8 *
CRTFile::chipData(isize nr) const
{
    return chips[nr]+0x10;
}

u16 
CRTFile::chipSize(isize nr) const
{
    return LO_HI(chips[nr][0xF], chips[nr][0xE]);
}

u16
CRTFile::chipType(isize nr) const
{
    return LO_HI(chips[nr][0x9], chips[nr][0x8]);
}

u16
CRTFile::chipBank(isize nr) const
{
    return LO_HI(chips[nr][0xB], chips[nr][0xA]);
}

u16
CRTFile::chipAddr(isize nr) const
{
    return LO_HI(chips[nr][0xD], chips[nr][0xC]);
}

void
CRTFile::dump() const
{
    msg("Cartridge: %s\n", getName().c_str());
    msg("   Header: %08X bytes (normally 0x40)\n", headerSize());
    msg("   Type:   %ld\n", (long)cartridgeType());
    msg("   Game:   %d\n", initialGameLine());
    msg("   Exrom:  %d\n", initialExromLine());
}

void
CRTFile::repair()
{
    //
    // General errors
    //

    // Some cartridges show a header size of 0x20 which is wrong
    if (headerSize() < 0x40) {
        u32 newSize = 0x40;
        data[0x10] = BYTE3(newSize);
        data[0x11] = BYTE2(newSize);
        data[0x12] = BYTE1(newSize);
        data[0x13] = BYTE0(newSize);
    }
    
    //
    // Individual errors
    //
    
    switch (fnv64()) {

        case 0xb2a479a5a2ee6cd5: // Mikro Assembler

            // Replace invalid CRT type $00 by $1C
            debug(CRT_DEBUG, "Repairing broken Mikro Assembler cartridge\n");
            data[0x17] = 0x1C;
            break;
    }
}

}
