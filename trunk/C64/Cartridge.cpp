/*
 * (C) 2009 A. Carl Douglas. All rights reserved.
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

#include "Cartridge.h"

Cartridge::Cartridge()
{
    data = NULL;
    dealloc();
}

Cartridge::~Cartridge()
{
	dealloc();
}

void
Cartridge::dealloc()
{
    if (data) {
        free(data);
        data = NULL;
    }
    
    for (unsigned i = 0; i < 64; i++)
        chips[i] = NULL;
    
    numberOfChips = 0;
}

bool
Cartridge::isCRTFile(const char *filename)
{
    int magic_bytes[] = { 'C','6','4',' ','C','A','R','T','R','I','D','G','E',' ',' ',' ',EOF };
    
    assert(filename != NULL);
    
    if (!checkFileSuffix(filename, ".CRT") && !checkFileSuffix(filename, ".crt"))
        return false;
    
    if (!checkFileSize(filename, 0x40, -1))
        return false;
    
    if (!checkFileHeader(filename, magic_bytes))
        return false;
    
    return true;
}

Cartridge *
Cartridge::cartridgeFromFile(const char *filename)
{
    Cartridge *cartridge;
    
    cartridge = new Cartridge();
    if (!cartridge->readFromFile(filename)) {
        delete cartridge;
        cartridge = NULL;
    }
    return cartridge;
    
}

bool
Cartridge::readFromBuffer(const void *buffer, unsigned length)
{
    if ((data = (uint8_t *)malloc(length)) == NULL) {
        return false;
    }
    memcpy(data, buffer, length);
    
    // Scan cartridge header
    if (memcmp("C64 CARTRIDGE   ", data, 16) != 0) {
        fprintf(stderr, "Bad cartridge signature. Expected 'C64  CARTRIDGE  ', got ...\n");
        printReadable(&data[0], 16);
        return false;
    }
    
    // Cartridge header size
    uint32_t headerSize = HI_HI_LO_LO(data[0x10],data[0x11],data[0x12],data[0x13]);
    
    // Minimum header size is 0x40. Some cartridges show a value of 0x20 which is wrong.
    if (headerSize < 0x40) headerSize = 0x40;
    
    fprintf(stderr, "Cartridge: %s\n", getCartridgeName());
    fprintf(stderr, "   Header: %08X bytes long (normally 0x40)\n", headerSize);
    fprintf(stderr, "   Type:   %d\n", getCartridgeType());
    fprintf(stderr, "   Game:   %d\n", getGameLine());
    fprintf(stderr, "   Exrom:  %d\n", getExromLine());
    
    // Load chip packets
    uint8_t *ptr = &data[headerSize];
    for (numberOfChips = 0; ptr < data + length; numberOfChips++) {
        
        if (memcmp("CHIP", ptr, 4) != 0) {
            fprintf(stderr, "Unexpected data in cartridge, expected 'CHIP'\n");
            printReadable(ptr, 4);
            return false;
        }
        
        // Remember start address of each chip section
        chips[numberOfChips] = ptr;
        
        ptr += 0x10;
        ptr += getChipSize(numberOfChips);
    }
    
    fprintf(stderr, "CRT container imported successfully (%d chips)\n", numberOfChips);
    return true;	
}
