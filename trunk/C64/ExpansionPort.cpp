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

    cartridgeAttached = false;
    gameLine = true;
    exromLine = true;
    
    for (unsigned i = 0; i < 64; i++) {
        chip[i] = NULL;
        chipStartAddress[i] = 0;
        chipSize[i] = 0;
    }
}

void
ExpansionPort::ping()
{
    c64->putMessage(MSG_CARTRIDGE, c64->isCartridgeAttached());
    // IN FUTURE:
    // c64->putMessage(MSG_CARTRIDGE, cartridgeAttached);
}

void
ExpansionPort::loadFromBuffer(uint8_t **buffer)
{
    debug(2, "  Loading expansion port state...\n");
    debug(2, "  IMPLEMENTEATION MISSING...\n");
}

void
ExpansionPort::saveToBuffer(uint8_t **buffer)
{
    debug(2, "  Saving expansion port state...\n");
    debug(2, "  IMPLEMENTEATION MISSING...\n");
}

void
ExpansionPort::dumpState()
{
    msg("Expansion port\n");
    msg("--------------\n");
    msg("(no output)\n");
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
    
    cartridgeAttached = true;
    gameLine = c->gameIsHigh();
    exromLine = c->exromIsHigh();

    // Load chip packets
    for (unsigned i = 0; i < c->getNumberOfChips(); i++) {
        attachChip(i, c);
    }
    
    // if(numberOfChips > 0) {
    //     switchBank(0);
    // }
    
    debug(1,"%d chips imported successfully\n", c->getNumberOfChips());
    return true;
}

void
ExpansionPort::detachCartridge()
{
    for (unsigned i = 0; i < 64; i++) {
        if (chip[i])
            free(chip[i]);
    }

    reset();
}

bool
ExpansionPort::isRomAddr(uint16_t addr)
{
    // TODO
    return false;
}

uint8_t
ExpansionPort::peek(uint16_t addr)
{
    // TODO
    return 0;
}

void
ExpansionPort::poke(uint16_t addr, uint8_t value)
{
}






