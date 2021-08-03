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
#include "IO.h"

bool
CRTFile::isCompatibleName(const std::string &name)
{
    auto s = util::extractSuffix(name);
    return s == "crt" || s == "CRT";
}

bool
CRTFile::isCompatibleStream(std::istream &stream)
{
    const u8 magicBytes[] = {
        'C','6','4',' ','C','A','R','T','R','I','D','G','E',' ',' ',' ' };
    
    if (util::streamLength(stream) < 0x40) return false;
    return util::matchingStreamHeader(stream, magicBytes, sizeof(magicBytes));
}

PETName<16>
CRTFile::getName() const
{
    return PETName<16>(data + 0x20, 0x00);
}

isize
CRTFile::readFromStream(std::istream &stream)
{
    usize result = AnyFile::readFromStream(stream);
    if (CRT_DEBUG) dump();
            
    // Load chip packets
    u8 *ptr = data + headerSize();
    for (numberOfChips = 0; ptr < data + size; numberOfChips++) {
        
        if (numberOfChips == MAX_PACKETS) {
            warn("CRT file contains too many chip packets. Aborting!\n");
            break;
        }
        
        if (memcmp("CHIP", ptr, 4) != 0) {
            warn("Unexpected data in cartridge, expected 'CHIP'\n");
            return result; // TODO: throw exception instead
        }
        
        // Remember start address of each chip section
        chips[numberOfChips] = ptr;
        
        ptr += 0x10;
        ptr += chipSize(numberOfChips);
    }
    
    plain("CRT file imported successfully (%zd chips)\n", numberOfChips);
    return result;
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
    
    switch (util::fnv_1a_64(data, size)) {

        case 0xb2a479a5a2ee6cd5: // Mikro Assembler

            // Replace invalid CRT type $00 by $1C
            msg("Repairing broken Mikro Assembler cartridge\n");
            data[0x17] = 0x1C;
            break;            
    }
}
