// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "CRTFile.h"
#include "Cartridge.h"
#include "Checksum.h"
#include "IOUtils.h"

bool
CRTFile::isCompatible(const string &path)
{
    return util::uppercased(util::extractSuffix(path)) == "CRT";
}

bool
CRTFile::isCompatible(std::istream &stream)
{
    return util::matchingStreamHeader(stream, "C64 CARTRIDGE   ");
}

PETName<16>
CRTFile::getName() const
{
    return PETName<16>(data + 0x20, 0x00);
}

void
CRTFile::finalizeRead()
{
    if constexpr (CRT_DEBUG) dump();
            
    // Fix known inconsistencies
    repair();

    // Load chip packets
    u8 *ptr = data + headerSize();
    for (numberOfChips = 0; ptr < data + size; numberOfChips++) {
        
        if (numberOfChips == MAX_PACKETS) {
            throw VC64Error(ERROR_CRT_TOO_MANY_PACKETS);
        }
        if (memcmp("CHIP", ptr, 4) != 0) {
            throw VC64Error(ERROR_CRT_CORRUPTED_PACKET);
        }
        
        // Remember start address of each chip section
        chips[numberOfChips] = ptr;
        
        ptr += 0x10;
        ptr += chipSize(numberOfChips);
    }
    
    plain(CRT_DEBUG, "CRT file imported (%ld chips)\n", numberOfChips);
}

CartridgeType
CRTFile::cartridgeType() const {
    
    u16 type = LO_HI(data[0x17], data[0x16]);
    return CartridgeType(type);
}

bool
CRTFile::isSupported() const
{
    return Cartridge::isSupportedType(cartridgeType());
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
    
    switch (util::fnv64(data, size)) {

        case 0xb2a479a5a2ee6cd5: // Mikro Assembler

            // Replace invalid CRT type $00 by $1C
            debug(CRT_DEBUG, "Repairing broken Mikro Assembler cartridge\n");
            data[0x17] = 0x1C;
            break;            
    }
}
