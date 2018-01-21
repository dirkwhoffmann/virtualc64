/*
 * Original implementation by A. Carl Douglas, 2009
 * Modified and maintained by Dirk W. Hoffmann
 * All rights reserved
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

#include "CRTContainer.h"

CRTContainer::CRTContainer()
{
    data = NULL;
    dealloc();
}

CRTContainer::~CRTContainer()
{
	dealloc();
}

void
CRTContainer::dealloc()
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
CRTContainer::isCRTFile(const char *filename)
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

CRTContainer *
CRTContainer::containerFromCRTFile(const char *filename)
{
    CRTContainer *cartridge;
    
    cartridge = new CRTContainer();
    if (!cartridge->readFromFile(filename)) {
        delete cartridge;
        cartridge = NULL;
    }
    return cartridge;
    
}

bool
CRTContainer::readFromBuffer(const uint8_t *buffer, unsigned length)
{
    if ((data = (uint8_t *)malloc(length)) == NULL) {
        return false;
    }
    memcpy(data, buffer, length);
    
    // Scan cartridge header
    if (memcmp("C64 CARTRIDGE   ", data, 16) != 0) {
        warn("Bad cartridge signature. Expected 'C64  CARTRIDGE  ', got ...\n");
        printReadable(&data[0], 16);
        return false;
    }
    
    // Cartridge header size
    uint32_t headerSize = HI_HI_LO_LO(data[0x10],data[0x11],data[0x12],data[0x13]);
    
    // Minimum header size is 0x40. Some cartridges show a value of 0x20 which is wrong.
    if (headerSize < 0x40) headerSize = 0x40;
    
    msg("Cartridge: %s\n", getCartridgeName());
    msg("   Header: %08X bytes long (normally 0x40)\n", headerSize);
    msg("   Type:   %d\n", getCartridgeType());
    msg("   Game:   %d\n", getGameLine());
    msg("   Exrom:  %d\n", getExromLine());
    
    // Load chip packets
    uint8_t *ptr = &data[headerSize];
    for (numberOfChips = 0; ptr < data + length; numberOfChips++) {
        
        if (memcmp("CHIP", ptr, 4) != 0) {
            warn("Unexpected data in cartridge, expected 'CHIP'\n");
            printReadable(ptr, 4);
            return false;
        }
        
        // Remember start address of each chip section
        chips[numberOfChips] = ptr;
        
        ptr += 0x10;
        ptr += getChipSize(numberOfChips);
    }
    
    debug("CRT container imported successfully (%d chips)\n", numberOfChips);
    return true;	
}

const char *
CRTContainer::getCartridgeTypeName()
{
    switch (getCartridgeType()) {
            
        case CRT_NORMAL: return "Normal cartridge";
        case CRT_ACTION_REPLAY: return "Action Replay";
        case CRT_KCS_POWER_CARTRIDGE: return "KCS Power Cartridge";
        case CRT_FINAL_CARTRIDGE_III: return "Final Cartridge III";
        case CRT_SIMONS_BASIC: return "Simons Basic";
        case CRT_OCEAN_TYPE_1: return "Ocean type 1";
        case CRT_EXPORT_CARTRIDGE: return "Expert Cartridge";
        case CRT_FUN_PLAY_POWER_PLAY: return "Fun Play, Power Play";
        case CRT_SUPER_GAMES: return "Super Games";
        case CRT_ATOMIC_POWER: return "Atomic Power";
        case CRT_EPYX_FASTLOAD: return "Epyx Fastload";
        case CRT_WESTERMANN_LEARNING: return "Westermann Learning";
        case CRT_REX_UTILITY: return "Rex Utility";
        case CRT_FINAL_CARTRIDGE_I: return "Final Cartridge I";
        case CRT_MAGIC_FORMEL: return "Magic Formel";
        case CRT_C64_GAME_SYSTEM_SYSTEM_3: return "C64 Game System, System 3";
        case CRT_WARPSPEED: return "WarpSpeed";
        case CRT_DINAMIC: return "Dinamic";
        case CRT_ZAXXON_SUPER_ZAXXON: return "Zaxxon, Super Zaxxon (SEGA)";
        case CRT_MAGIC_DESK_DOMARK_HES_AUSTRALIA: return "Magic Desk, Domark, HES Australia";
        case CRT_SUPER_SNAPSHOT_5: return "Super Snapshot 5";
        case CRT_COMAL: return "Comal-80";
        case CRT_STRUCTURE_BASIC: return "Structured Basic";
        case CRT_ROSS: return "Ross";
        case CRT_DELA_EP64: return "Dela EP64";
        case CRT_DELA_EP7x8: return "Dela EP7x8";
        case CRT_DELA_EP256: return "Dela EP256";
        case CRT_REX_EP256: return "Rex EP256";
        default: return "";
    }
}


