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
CRTFile::isCompatibleName(const std::string &name)
{
    auto s = suffix(name);
    return s == "crt" || s == "CRT";
}

bool
CRTFile::isCompatibleStream(std::istream &stream)
{
    if (streamLength(stream) < 0x40) return false;
    return matchingStreamHeader(stream, magicBytes, sizeof(magicBytes));
}

bool
CRTFile::isCompatibleBuffer(const u8 *buffer, size_t length)
{
    if (length < 0x40) return false;
    return matchingBufferHeader(buffer, magicBytes, sizeof(magicBytes));
}

bool
CRTFile::isCompatibleFile(const char *path)
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
    return isCompatibleBuffer(buf, len);
}

bool
CRTFile::matchingFile(const char *path)
{
    return isCompatibleFile(path);
}

usize
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
    
    msg("CRT file imported successfully (%d chips)\n", numberOfChips);
    return result;
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

void
CRTFile::dump()
{
    msg("Cartridge: %s\n", getName());
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
    
    // Compute a fingerprint for the CRT file
    u64 fingerprint = fnv_1a_64(data, size);
    printf("CRT fingerprint: %llx\n", fingerprint);
    debug(CRT_DEBUG, "CRT fingerprint: %llx\n", fingerprint);

    // Check for known inconsistencies
    switch (fingerprint) {

        case 0xb2a479a5a2ee6cd5: // Mikro Assembler (invalid CRT type)

            // Replace invalid CRT type $00 by $1C
            msg("Repairing broken Mikro Assembler cartridge\n");
            data[0x17] = 0x1C;
            break;            
    }
}
