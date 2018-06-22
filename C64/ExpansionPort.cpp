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

ExpansionPort::ExpansionPort()
{
    setDescription("Expansion port");
    debug(3, "  Creating expansion port at address %p...\n", this);

    cartridge = NULL;
    gameLine = 1;
    exromLine = 1;
}

ExpansionPort::~ExpansionPort()
{
    debug(3, "  Releasing expansion port...\n");
    detachCartridge();
}

void
ExpansionPort::reset()
{
    VirtualComponent::reset();
    
    if (cartridge) {
        cartridge->reset();
        setGameLine(cartridge->getInitialGameLine());
        setExromLine(cartridge->getInitialExromLine());
    } else {
        setGameLine(1);
        setExromLine(1);
    }
    
    // debug("Resetting port: game = %d exrom = %d\n", cartridge->getInitialGameLine(), cartridge->getInitialExromLine());
}

void
ExpansionPort::ping()
{
    VirtualComponent::ping();
    c64->putMessage(cartridge ? MSG_CARTRIDGE : MSG_NO_CARTRIDGE);
}

size_t
ExpansionPort::stateSize()
{
    return 4 + (cartridge ? cartridge->stateSize() : 0);
}

void
ExpansionPort::loadFromBuffer(uint8_t **buffer)
{
    uint8_t *old = *buffer;
    
    // Delete old cartridge (if any)
    if (cartridge != NULL) {
        delete cartridge;
        cartridge = NULL;
    }
    
    // Read cartridge type
    CartridgeType cartridgeType = (CartridgeType)read16(buffer);
    exromLine = read8(buffer);
    gameLine = read8(buffer);
    
    // Read cartridge data (if any)
    if (cartridgeType != CRT_NONE) {
        cartridge = Cartridge::makeCartridgeWithType(c64, cartridgeType);
        cartridge->loadFromBuffer(buffer);
    }
    
    debug(2, "  Expansion port state loaded (%d bytes)\n", *buffer - old);
    assert(*buffer - old == stateSize());
}

void
ExpansionPort::saveToBuffer(uint8_t **buffer)
{
    uint8_t *old = *buffer;
    
    write16(buffer, cartridge ? cartridge->getCartridgeType() : CRT_NONE);
    write8(buffer, exromLine);
    write8(buffer, gameLine);
    
    // Write cartridge data (if any)
    if (cartridge != NULL)
        cartridge->saveToBuffer(buffer);
 
    debug(4, "  Expansion port state saved (%d bytes)\n", *buffer - old);
    assert(*buffer - old == stateSize());
}

void
ExpansionPort::dumpState()
{
    msg("Expansion port\n");
    msg("--------------\n");
    
    msg("Game line:  %d\n", gameLine);
    msg("Exrom line: %d\n", exromLine);

    
    if (cartridge == NULL) {
        msg("No cartridge attached\n");
    } else {
        cartridge->dumpState();
    }
}

CartridgeType
ExpansionPort::getCartridgeType()
{
    return cartridge ? cartridge->getCartridgeType() : CRT_NONE;
}

bool
ExpansionPort::romIsBlendedIn(uint16_t addr)
{
    
    return cartridge ? cartridge->romIsBlendedIn(addr) : false;
}

uint8_t
ExpansionPort::peek(uint16_t addr)
{
    return cartridge ? cartridge->peek(addr) : 0;
}

uint8_t
ExpansionPort::read(uint16_t addr)
{
    return cartridge ? cartridge->read(addr) : 0;
}

uint8_t
ExpansionPort::peekIO1(uint16_t addr)
{
    /* "Die beiden mit "I/O 1" und "I/O 2" bezeichneten Bereiche
     *  sind für Erweiterungskarten reserviert und normalerweise ebenfalls offen,
     *  ein Lesezugriff liefert auch hier "zufällige" Daten (dass diese Daten gar
     *  nicht so zufällig sind, wird in Kapitel 4 noch ausführlich erklärt. Ein
     *  Lesen von offenen Adressen liefert nämlich auf vielen C64 das zuletzt vom
     *  VIC gelesene Byte zurück!)" [C.B.]
     */
    return cartridge ? cartridge->peekIO1(addr) : c64->vic.dataBus;
}

uint8_t
ExpansionPort::readIO1(uint16_t addr)
{
    return cartridge ? cartridge->readIO1(addr) : c64->vic.dataBus;
}

uint8_t
ExpansionPort::peekIO2(uint16_t addr)
{
    return cartridge ? cartridge->peekIO2(addr) : c64->vic.dataBus;
}

uint8_t
ExpansionPort::readIO2(uint16_t addr)
{
    return cartridge ? cartridge->readIO2(addr) : c64->vic.dataBus;
}

void
ExpansionPort::pokeIO1(uint16_t addr, uint8_t value)
{
    assert(addr >= 0xDE00 && addr <= 0xDEFF);
    if (cartridge != NULL)
        cartridge->pokeIO1(addr, value);
}

void
ExpansionPort::pokeIO2(uint16_t addr, uint8_t value)
{
    assert(addr >= 0xDF00 && addr <= 0xDFFF);
    if (cartridge != NULL)
        cartridge->pokeIO2(addr, value);
}

void
ExpansionPort::setGameLine(bool value)
{
    gameLine = value;
    c64->mem.updatePeekPokeLookupTables();
}

void
ExpansionPort::setExromLine(bool value)
{
    exromLine = value;
    c64->mem.updatePeekPokeLookupTables();
}

bool
ExpansionPort::attachCartridge(Cartridge *c)
{
    assert(c != NULL);
    
    // Remove old cartridge (if any) and assign new one
    detachCartridge();
    cartridge = c;
    
    // Reset cartridge to update exrom and game line on the expansion port
    cartridge->reset();
    
    c64->putMessage(MSG_CARTRIDGE);
    debug(1, "Cartridge attached to expansion port");
    cartridge->dumpState();

    return true;
}

/*
bool
ExpansionPort::attachCartridge(CRTContainer *c)
{
    assert(c != NULL);
    Cartridge *cartridge = Cartridge::makeCartridgeWithCRTContainer(c64, c);

    if (cartridge == NULL) {
        warn("Cannot create Cartridge from CRTContainer");
        return false;
    }
    
    return attachCartridge(cartridge);
}
*/

void
ExpansionPort::detachCartridge()
{
    if (cartridge == NULL)
        return;
    
    delete cartridge;
    cartridge = NULL;
    
    setGameLine(1);
    setExromLine(1);

    c64->putMessage(MSG_NO_CARTRIDGE);
    
    debug(1, "Cartridge detached from expansion port");
}


