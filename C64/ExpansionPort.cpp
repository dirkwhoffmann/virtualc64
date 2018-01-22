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
 
    if (cartridge)
        cartridge->reset();
}

void
ExpansionPort::ping()
{
    c64->putMessage(MSG_CARTRIDGE, cartridge != NULL);
}

uint32_t
ExpansionPort::stateSize()
{
    return 2 + (cartridge ? cartridge->stateSize() : 0);
}

void
ExpansionPort::loadFromBuffer(uint8_t **buffer)
{
    uint8_t *old = *buffer;
    
    detachCartridge();
    
    // Read cartridge type
    CartridgeType cartridgeType = (CartridgeType)read16(buffer);

    // Read cartridge data (if any)
    if (cartridgeType != CRT_NONE) {
        attachCartridge(buffer, cartridgeType);
    }
    
    debug(2, "  Expansion port state loaded (%d bytes)\n", *buffer - old);
    assert(*buffer - old == stateSize());
}

void
ExpansionPort::saveToBuffer(uint8_t **buffer)
{
    uint8_t *old = *buffer;
    
    // Write cartridge type
    write16(buffer, cartridge ? cartridge->getCartridgeType() : CRT_NONE);

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

    if (cartridge == NULL) {
        msg("No cartridge attached");
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
ExpansionPort::romIsBlendedIn(uint16_t addr) {
    
    return cartridge ? cartridge->romIsBlendedIn(addr) : false;
}

uint8_t
ExpansionPort::peek(uint16_t addr)
{
    return cartridge ? cartridge->peek(addr) : 0;
}

uint8_t
ExpansionPort::peekIO(uint16_t addr)
{
    /* "Die beiden mit "I/O 1" und "I/O 2" bezeichneten Bereiche
     *  sind für Erweiterungskarten reserviert und normalerweise ebenfalls offen,
     *  ein Lesezugriff liefert auch hier "zufällige" Daten (dass diese Daten gar
     *  nicht so zufällig sind, wird in Kapitel 4 noch ausführlich erklärt. Ein
     *  Lesen von offenen Adressen liefert nämlich auf vielen C64 das zuletzt vom
     *  VIC gelesene Byte zurück!)" [C.B.]
     */
   return cartridge ? cartridge->peekIO(addr) : c64->vic.getDataBus();
}

void
ExpansionPort::poke(uint16_t addr, uint8_t value)
{
    assert(addr >= 0xDE00 && addr <= 0xDFFF);
    
    if (cartridge != NULL)
        cartridge->poke(addr, value);
}

bool
ExpansionPort::getGameLine() {

    // GameLine is true, if no cartridge is attached
    return cartridge ? cartridge->getGameLine() : true;    
}

void
ExpansionPort::gameLineHasChanged()
{
    assert(c64 != NULL);
    c64->mem.updatePeekPokeLookupTables();
}

bool
ExpansionPort::getExromLine()
{
    // ExromLine is true, if no cartridge is attached
    return cartridge ? cartridge->getExromLine() : true;
}

void
ExpansionPort::exromLineHasChanged()
{
    assert(c64 != NULL);
    c64->mem.updatePeekPokeLookupTables();
}

bool
ExpansionPort::attachCartridge(Cartridge *c)
{
    assert(c != NULL);
    
    detachCartridge();
    
    cartridge = c;
    cartridge->powerup();
    
    c64->putMessage(MSG_CARTRIDGE, 1);
    
    debug(1, "Cartridge attached to expansion port");
    cartridge->dumpState();

    return true;
}

bool
ExpansionPort::attachCartridge(uint8_t **buffer, CartridgeType type)
{
    assert(buffer != NULL);
    Cartridge *cartridge = Cartridge::makeCartridgeWithBuffer(c64, buffer, type);
    
    if (cartridge == NULL) {
        warn("Cannot create Cartridge from data buffer");
        return false;
    }
    
    return attachCartridge(cartridge);
}

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

void
ExpansionPort::detachCartridge()
{
    if (cartridge == NULL)
        return;
    
    delete cartridge;
    cartridge = NULL;
    
    gameLineHasChanged();
    exromLineHasChanged();
    c64->putMessage(MSG_CARTRIDGE, 0);
    
    debug(1, "Cartridge detached from expansion port");
}



