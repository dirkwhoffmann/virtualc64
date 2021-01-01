// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "CRTFile.h"
#include "Cartridge.h"

const u8 CRTFile::magicBytes[] = {
    'C','6','4',' ','C','A','R','T','R','I','D','G','E',' ',' ',' ' };

bool
CRTFile::isCRTBuffer(const u8 *buffer, size_t length)
{
    if (length < 0x40) return false;
    return matchingBufferHeader(buffer, magicBytes, sizeof(magicBytes));
}

CartridgeType
CRTFile::typeOfCRTBuffer(const u8 *buffer, size_t length)
{
    assert(isCRTBuffer(buffer, length));
    return (CartridgeType)LO_HI(buffer[0x17], buffer[0x16]);
}

bool
CRTFile::isSupportedCRTBuffer(const u8 *buffer, size_t length)
{
    if (!isCRTBuffer(buffer, length))
        return false;
    return Cartridge::isSupportedType(typeOfCRTBuffer(buffer, length));
}

bool
CRTFile::isUnsupportedCRTBuffer(const u8 *buffer, size_t length)
{
    return isCRTBuffer(buffer, length) && !isSupportedCRTBuffer(buffer, length);
}

bool
CRTFile::isCRTFile(const char *path)
{
    assert(path != NULL);
    
    if (!checkFileSuffix(path, ".CRT") && !checkFileSuffix(path, ".crt"))
        return false;
    
    if (!checkFileSize(path, 0x40, -1))
        return false;
    
    if (!matchingFileHeader(path, magicBytes, sizeof(magicBytes)))
        return false;
    
    return true;
}

CRTFile::CRTFile()
{
    memset(chips, 0, sizeof(chips));
}

void
CRTFile::dealloc()
{
    AnyFile::dealloc();
    memset(chips, 0, sizeof(chips));
    numberOfChips = 0;
}
        
bool
CRTFile::matchingBuffer(const u8 *buf, size_t len)
{
    return isCRTBuffer(buf, len);
}

bool
CRTFile::matchingFile(const char *path)
{
    return isCRTFile(path);
}

bool
CRTFile::oldReadFromBuffer(const u8 *buffer, size_t length)
{
    if (!AnyFile::oldReadFromBuffer(buffer, length))
        return false;
    
    // Only proceed if the cartridge header matches
    if (memcmp("C64 CARTRIDGE   ", data, 16) != 0) {
        warn("Bad cartridge signature. Expected 'C64  CARTRIDGE  '\n");
        return false;
    }

    // Some CRT files contain incosistencies. We try to fix them here.
    if (!repair()) {
        warn("Failed to repair broken CRT file\n");
        return false;
    }

    // Cartridge header size
    u32 headerSize = HI_HI_LO_LO(data[0x10],data[0x11],data[0x12],data[0x13]);
    
    // Minimum header size is 0x40. Some cartridges show a value of 0x20 which is wrong.
    if (headerSize < 0x40) headerSize = 0x40;
    
    msg("Cartridge: %s\n", getName());
    msg("   Header: %08X bytes long (normally 0x40)\n", headerSize);
    msg("   Type:   %ld\n", (long)cartridgeType());
    msg("   Game:   %d\n", initialGameLine());
    msg("   Exrom:  %d\n", initialExromLine());
    
    // Load chip packets
    u8 *ptr = &data[headerSize];
    for (numberOfChips = 0; ptr < data + length; numberOfChips++) {
        
        if (numberOfChips == MAX_PACKETS) {
            warn("CRT file contains too many chip packets. Aborting!\n");
            break;
        }
        
        if (memcmp("CHIP", ptr, 4) != 0) {
            warn("Unexpected data in cartridge, expected 'CHIP'\n");
            return false;
        }
        
        // Remember start address of each chip section
        chips[numberOfChips] = ptr;
        
        ptr += 0x10;
        ptr += chipSize(numberOfChips);
    }
    
    msg("CRT file imported successfully (%d chips)\n", numberOfChips);
    return true;	
}

CartridgeType
CRTFile::cartridgeType() {
    
    u16 type = LO_HI(data[0x17], data[0x16]);
    return CartridgeType(type);
}

bool
CRTFile::isSupported()
{
    return Cartridge::isSupportedType(cartridgeType());
}

bool
CRTFile::repair()
{
    if ((data == NULL) != (size == 0)) {
        warn("CRT file inconsistency: data = %p size = %zu\n", data, size);
        return false;
    }

    // Compute a fingerprint for the CRT file
    u64 fingerprint = fnv_1a_64(data, size);
    trace(CRT_DEBUG, "CRT fingerprint: %llx\n", fingerprint);

    // Check for known inconsistencies
    switch (fingerprint) {

        case 0xb2a479a5a2ee6cd5: // Mikro Assembler (invalid CRT type)

            // Replace invalid CRT type $00 by $1C
            msg("Repairing broken Mikro Assembler cartridge\n");
            data[0x17] = 0x1C;
            break;
    }

    return true; 
}

