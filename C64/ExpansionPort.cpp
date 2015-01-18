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
}

ExpansionPort::~ExpansionPort()
{
    debug(2, "  Releasing expansion port...\n");

    // Iterate through all chips and free allocated memory
    // TODO
}

void
ExpansionPort::reset()
{
    debug(2, "  Resetting expansion port...\n");

    cartridgeAttached = false;
    gameLine = false;
    exromLine = false;
    
    for (unsigned i = 0; i < 64; i++) {
        chips[i] = NULL;
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

bool
ExpansionPort::attachCartridge(Cartridge *c)
{
    removeCartridge();
    
    cartridgeAttached = true;
    // gameLine = ??;
    // exromLine = ??;
    
    // TODO
    return true;
}

void
ExpansionPort::removeCartridge()
{
    for (unsigned i = 0; i < 64; i++) {
        if (chips[i]) free(chips[i]);
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






