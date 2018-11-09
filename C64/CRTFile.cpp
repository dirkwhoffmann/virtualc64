/*!
 * @file        CRTFile.cpp
 *              Original implementation by A. Carl Douglas, 2009
 *              Modified and maintained by Dirk W. Hoffmann
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   Dirk W. Hoffmann. All rights reserved.
 */
/* This program is free software; you can redistribute it and/or modify
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

#include "CRTFile.h"
#include "Cartridge.h"

const uint8_t CRTFile::magicBytes[] = {
    'C','6','4',' ','C','A','R','T','R','I','D','G','E',' ',' ',' ', 0x00 };

bool
CRTFile::isCRTBuffer(const uint8_t *buffer, size_t length)
{
    if (length < 0x40) return false;
    return checkBufferHeader(buffer, length, magicBytes);
}

CartridgeType
CRTFile::typeOfCRTBuffer(const uint8_t *buffer, size_t length)
{
    assert(isCRTBuffer(buffer, length));
    return (CartridgeType)LO_HI(buffer[0x17], buffer[0x16]);
}

const char *
CRTFile::typeNameOfCRTBuffer(const uint8_t *buffer, size_t length)
{
    CartridgeType type = typeOfCRTBuffer(buffer, length);
    return CRTFile::cartridgeTypeName(type);
}

bool
CRTFile::isSupportedCRTBuffer(const uint8_t *buffer, size_t length)
{
    if (!isCRTBuffer(buffer, length))
        return false;
    return Cartridge::isSupportedType(typeOfCRTBuffer(buffer, length));
}

bool
CRTFile::isUnsupportedCRTBuffer(const uint8_t *buffer, size_t length)
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
    
    if (!checkFileHeader(path, magicBytes))
        return false;
    
    return true;
}

const char *
CRTFile::cartridgeTypeName(CartridgeType type)
{
    switch (type) {
            
        case CRT_NORMAL: return "Normal cartridge";
        case CRT_ACTION_REPLAY: return "Action Replay";
        case CRT_KCS_POWER: return "KCS Power";
        case CRT_FINAL_III: return "Final Cartridge III";
        case CRT_SIMONS_BASIC: return "Simons Basic";
        case CRT_OCEAN: return "Ocean";
        case CRT_EXPERT: return "Expert";
        case CRT_FUNPLAY: return "Fun Play";
        case CRT_SUPER_GAMES: return "Super Games";
        case CRT_ATOMIC_POWER: return "Atomic Power";
        case CRT_EPYX_FASTLOAD: return "Epyx Fastload";
        case CRT_WESTERMANN: return "Westermann";
        case CRT_REX: return "REX";
        case CRT_FINAL_I: return "Final Cartridge I";
        case CRT_MAGIC_FORMEL: return "Magic Formel";
        case CRT_GAME_SYSTEM_SYSTEM_3: return "Game System 3";
        case CRT_WARPSPEED: return "WarpSpeed";
        case CRT_DINAMIC: return "Dinamic";
        case CRT_ZAXXON: return "Zaxxon (SEGA)";
        case CRT_MAGIC_DESK: return "Magic Desk";
        case CRT_SUPER_SNAPSHOT_V5: return "Super Snapshot";
        case CRT_COMAL80: return "Comal 80";
        case CRT_STRUCTURE_BASIC: return "Structured Basic";
        case CRT_ROSS: return "Ross";
        case CRT_DELA_EP64: return "Dela EP64";
        case CRT_DELA_EP7x8: return "Dela EP7x8";
        case CRT_DELA_EP256: return "Dela EP256";
        case CRT_REX_EP256: return "Rex EP256";
        case CRT_MIKRO_ASSEMBLER: return "Mikro Assembler";
        case CRT_FINAL_PLUS: return "Final Plus";
        case CRT_ACTION_REPLAY4: return "Action replay 4";
        case CRT_STARDOS: return "Stardos";
        case CRT_EASYFLASH: return "Easyflash";
        case CRT_EASYFLASH_XBANK: return "Easyflash (XBank)";
        case CRT_CAPTURE: return "Capture";
        case CRT_ACTION_REPLAY3: return "Action replay 3";
        case CRT_RETRO_REPLAY: return "Metro replay";
        case CRT_MMC64: return "MMC 64";
        case CRT_MMC_REPLAY: return "MMC replay";
        case CRT_IDE64: return "IDE 64";
        case CRT_SUPER_SNAPSHOT: return "Super snapshot";
        case CRT_IEEE488: return "IEEE 488";
        case CRT_GAME_KILLER: return "Game killer";
        case CRT_P64: return "P64";
        case CRT_EXOS: return "Exos";
        case CRT_FREEZE_FRAME: return "Freeze frame";
        case CRT_FREEZE_MACHINE: return "Freeze machine";
        case CRT_SNAPSHOT64: return "Snapshot 64";
        case CRT_SUPER_EXPLODE_V5: return "Super explode V5";
        case CRT_MAGIC_VOICE: return "Magic voice";
        case CRT_ACTION_REPLAY2: return "Action replay 2";
        case CRT_MACH5: return "Mach 5";
        case CRT_DIASHOW_MAKER: return "Diashow Maker";
        case CRT_PAGEFOX: return "Pagefox";
        case CRT_KINGSOFT: return "Kingsoft";
        case CRT_SILVERROCK_128: return "Silverrock 128";
        case CRT_FORMEL64: return "Formel 64";
        case CRT_RGCD: return "RGCD";
        case CRT_RRNETMK3: return "RRNETMK3";
        case CRT_EASYCALC: return "Easy calc";
        case CRT_GMOD2: return "GMOD 2";
            
        default: return "";
    }
}

CRTFile::CRTFile()
{
    setDescription("CRTFile");
    memset(chips, 0, sizeof(chips));
}

CRTFile *
CRTFile::makeWithBuffer(const uint8_t *buffer, size_t length)
{
    CRTFile *cartridge = new CRTFile();
    
    if (!cartridge->readFromBuffer(buffer, length)) {
        delete cartridge;
        return NULL;
    }
    
    return cartridge;
}

CRTFile *
CRTFile::makeWithFile(const char *filename)
{
    CRTFile *cartridge = new CRTFile();
    
    if (!cartridge->readFromFile(filename)) {
        delete cartridge;
        return NULL;
    }
    
    return cartridge;
}

void
CRTFile::dealloc()
{
    AnyC64File::dealloc();
    memset(chips, 0, sizeof(chips));
    numberOfChips = 0;
}
        
bool
CRTFile::readFromBuffer(const uint8_t *buffer, size_t length)
{
    if (!AnyC64File::readFromBuffer(buffer, length))
        return false;
    
    // Scan cartridge header
    if (memcmp("C64 CARTRIDGE   ", data, 16) != 0) {
        warn("Bad cartridge signature. Expected 'C64  CARTRIDGE  '\n");
        return false;
    }
    
    // Cartridge header size
    uint32_t headerSize = HI_HI_LO_LO(data[0x10],data[0x11],data[0x12],data[0x13]);
    
    // Minimum header size is 0x40. Some cartridges show a value of 0x20 which is wrong.
    if (headerSize < 0x40) headerSize = 0x40;
    
    msg("Cartridge: %s\n", getName());
    msg("   Header: %08X bytes long (normally 0x40)\n", headerSize);
    msg("   Type:   %d\n", cartridgeType());
    msg("   Game:   %d\n", initialGameLine());
    msg("   Exrom:  %d\n", initialExromLine());
    
    // Load chip packets
    uint8_t *ptr = &data[headerSize];
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
    
    debug("CRT file imported successfully (%d chips)\n", numberOfChips);
    return true;	
}

CartridgeType
CRTFile::cartridgeType() {
    
    uint16_t type = LO_HI(data[0x17], data[0x16]);
    return CartridgeType(type);
}

const char *
CRTFile::cartridgeTypeName()
{
    return cartridgeTypeName(cartridgeType());
}

