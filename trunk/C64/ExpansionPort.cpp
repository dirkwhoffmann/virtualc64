/*
 * Written by Dirk Hoffmann based on the original code by A. Carl Douglas.
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

ExpansionPort::ExpansionPort(C64 *c64)
{
    name = "Expansion port";
    
    debug(2, "  Creating expansion port at address %p...\n", this);
    this->c64 = c64;
    
    // We reset the expansion port here as it is *not* reset when resetting the emulator
    reset();
}

ExpansionPort::~ExpansionPort()
{
    debug(2, "  Releasing expansion port...\n");
    detachCartridge();
}

void
ExpansionPort::reset()
{
    debug(2, "  Resetting expansion port...\n");

    type = CRT_NONE;
    gameLine = true;
    exromLine = true;
    
    memset(rom, 0, sizeof(rom));
    memset(blendedIn, 0, sizeof(blendedIn));
    
    
    for (unsigned i = 0; i < 64; i++) {
        chip[i] = NULL;
        chipStartAddress[i] = 0;
        chipSize[i] = 0;
    }
}

void
ExpansionPort::softreset()
{
    debug(2, "  Soft-resetting expansion port...\n");
    
    if (chip[0])
        switchBank(0);
}

void
ExpansionPort::ping()
{
    c64->putMessage(MSG_CARTRIDGE, c64->isCartridgeAttached());
}

uint32_t
ExpansionPort::stateSize()
{
    uint32_t size = 3;
    
    for (unsigned i = 0; i < 64; i++)
        size += 4 + chipSize[i];

    size += sizeof(rom);
    size += sizeof(blendedIn);
    
    return size;
}

void
ExpansionPort::loadFromBuffer(uint8_t **buffer)
{
    uint8_t *old = *buffer;
    
    type = read8(buffer);
    gameLine = (bool)read8(buffer);
    exromLine = (bool)read8(buffer);

    for (unsigned i = 0; i < 64; i++) {
        chipStartAddress[i] = read16(buffer);
        chipSize[i] = read16(buffer);
        
        if (chipSize[i] > 0) {
            chip[i] = (uint8_t *)malloc(chipSize[i]);
            readBlock(buffer, chip[i], chipSize[i]);
        } else {
            chip[i] = NULL;
        }
    }

    readBlock(buffer, rom, sizeof(rom));
    readBlock(buffer, blendedIn, sizeof(blendedIn));
    
    debug(2, "  Expansion port state loaded (%d bytes)\n", *buffer - old);
    assert(*buffer - old == stateSize());
}

void
ExpansionPort::saveToBuffer(uint8_t **buffer)
{
    uint8_t *old = *buffer;
    
    write8(buffer, type);
    write8(buffer, (uint8_t)gameLine);
    write8(buffer, (uint8_t)exromLine);
    
    for (unsigned i = 0; i < 64; i++) {
        write16(buffer, chipStartAddress[i]);
        write16(buffer, chipSize[i]);
        
        if (chipSize[i] > 0) {
            writeBlock(buffer, chip[i], chipSize[i]);
        }
    }
    
    writeBlock(buffer, rom, sizeof(rom));
    writeBlock(buffer, blendedIn, sizeof(blendedIn));

    debug(2, "  Expansion port state saved (%d bytes)\n", *buffer - old);
    assert(*buffer - old == stateSize());
}

void
ExpansionPort::dumpState()
{
    msg("Expansion port\n");
    msg("--------------\n");

    if (!getCartridgeAttached()) {
        msg("No cartridge attached");
        return;
    }
    
    msg("Cartridge type: %d\n", getCartridgeType());
    msg("Game line:      %d\n", getGameLine());
    msg("Exrom line:     %d\n", getExromLine());
    
    for (unsigned i = 0; i < 64; i++) {
        if (chip[i] != NULL) {
            msg("Chip %2d:        %d KB starting at $%04X\n", i, chipSize[i] / 1024, chipStartAddress[i]);
        }
    }
}

void ExpansionPort::poke(uint16_t addr, uint8_t value)
{
    uint8_t bankNumber;
    
    // printf("ExpansionPort::poke %d,%d\n", addr, value);
    
    assert(addr >= 0xDE00 && addr <= 0xDFFF);
    
    if (!getCartridgeAttached())
        return;
    
    // For some cartridges like Simons basic, bank switching is triggered by writing
    // into I/O area 1 (0xDE00 - 0xDEFF) or I/O area 2 (0xDF00 - 0xDFFF)
    
    // Why do we need to store the written value here?
    rom[addr & 0x7FFF] = value;
    
    switch (type) {
        case CRT_NORMAL:
            break;
            
        case CRT_SIMONS_BASIC:
            if (addr == 0xDE00) {
                // Simon banks the second chip into $A000-BFFF
                if (value == 0x01) {
                    debug(3, "Simons basic: Writing %d into $DE00\n", value);
                    switchBank(1);
                } else {
                    debug(3, "Simons basic: Writing %d into $DE00\n", value);
                    // $A000-BFFF is additional RAM
                    // We need to remove the chip?!
                }
            }
            
        case CRT_C64_GAME_SYSTEM_SYSTEM_3:
            bankNumber = addr - 0xDE00;
            //  Huh? Bank numbers greater than 63 can occur?
            switchBank(bankNumber);
            break;
            
        case CRT_OCEAN_TYPE_1:
            printf("Switching...\n");
            bankNumber = value & 0x3F;
            switchBank(bankNumber);
            break;
            
#if 0
        case CRT_FUN_PLAY_POWER_PLAY:
            // TODO
            break;
#endif
            
        default:
            warn("Unsupported cartridge (type %d)\n", type);
    }
}

void
ExpansionPort::setGameLine(bool value)
{
    gameLine = value;
    c64->mem->updatePeekPokeLookupTables();
}

void
ExpansionPort::setExromLine(bool value)
{
    exromLine = value;
    c64->mem->updatePeekPokeLookupTables();
}

void
ExpansionPort::switchBank(unsigned nr)
{
    if (chip[nr] == NULL) {
        warn("Chip %d does not exist (cannot switch)", nr);
        return;
    }
    
    uint16_t loadAddr = chipStartAddress[nr];
    uint16_t size = chipSize[nr];
    
    if (0xFFFF - loadAddr < size) {
        warn("Chip %d covers an invalid memory area (start: %04X size: %d KB)", nr, loadAddr, size / 1024);
        return;
    }
    
    debug(1, "Switching to bank %d (start: %04X size: %d KB)", nr, loadAddr, size / 1024);
    memcpy(rom + loadAddr - 0x8000, chip[nr], size);
    for (unsigned i = loadAddr >> 12; i < (loadAddr + size) >> 12; i++) {
        assert (i < 16);
        blendedIn[i] = 1;
    }
}

void
ExpansionPort::attachChip(unsigned nr, Cartridge *c)
{
    assert(nr < 64);
    
    if (chip[nr])
        free(chip[nr]);
    
    if (!(chip[nr] = (uint8_t *)malloc(c->getChipSize(nr))))
        return;
    
    chipStartAddress[nr] = c->getChipAddr(nr);
    chipSize[nr] = c->getChipSize(nr);
    memcpy(chip[nr], c->getChipData(nr), c->getChipSize(nr));
    
    debug(1, "Chip %d is now in place: %d KB starting at $%04X (type: %d bank:%X)\n",
          nr, chipSize[nr] / 1024, chipStartAddress[nr], c->getChipType(nr), c->getChipBank(nr));
}

bool
ExpansionPort::attachCartridge(Cartridge *c)
{
    detachCartridge();
    
    type = c->getCartridgeType();
    gameLine = c->getGameLine();
    exromLine = c->getExromLine();

    // Load chip packets
    for (unsigned i = 0; i < c->getNumberOfChips(); i++) {
        attachChip(i, c);
    }
    
    // Hopefully, we got at least one chip
    if(chip[0] == NULL) {
        warn("Cartridge does not contain any chips");
        return false;
    }

    // Blend in chip 0
    switchBank(0);
    c64->mem->updatePeekPokeLookupTables();
    dumpState();

    c64->putMessage(MSG_CARTRIDGE, 1);
    return true;
}

void
ExpansionPort::detachCartridge()
{
    for (unsigned i = 0; i < 64; i++) {
        if (chip[i])
            free(chip[i]);
    }
    
    c64->putMessage(MSG_CARTRIDGE, 0);
    reset();
}



