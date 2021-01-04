// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"

ExpansionPort::~ExpansionPort()
{
    detachCartridge();
}

void
ExpansionPort::_reset()
{
    RESET_SNAPSHOT_ITEMS

    if (cartridge) {
        cartridge->reset();
        cartridge->resetCartConfig();
    } else {
        setCartridgeMode(CRTMODE_OFF);
    }
}

size_t
ExpansionPort::_size()
{
    SerCounter counter;
    applyToPersistentItems(counter);
    applyToResetItems(counter);
    
    if (cartridge) counter.count += cartridge->size();
    return counter.count;
}

size_t
ExpansionPort::_load(u8 *buffer)
{
    SerReader reader(buffer);
    applyToPersistentItems(reader);
    applyToResetItems(reader);
    
    // Load cartridge (if any)
    if (crtType != CRT_NONE) {
        assert(cartridge != NULL);
        delete cartridge;
        cartridge = Cartridge::makeWithType(c64, crtType);
        reader.ptr += cartridge->load(reader.ptr);
    }
    
    trace(SNP_DEBUG, "Recreated from %ld bytes\n", reader.ptr - buffer);
    return reader.ptr - buffer;
}

size_t
ExpansionPort::_save(u8 *buffer)
{
    SerWriter writer(buffer);
    applyToPersistentItems(writer);
    applyToResetItems(writer);
        
    // Save cartridge (if any)
    if (crtType != CRT_NONE) {
        writer.ptr += cartridge->save(writer.ptr);
    }
    
    trace(SNP_DEBUG, "Serialized to %ld bytes\n", writer.ptr - buffer); \
    return writer.ptr - buffer;
}

void
ExpansionPort::_dump()
{
    msg("Expansion port\n");
    msg("--------------\n");
    
    msg(" Game line:  %d\n", gameLine);
    msg("Exrom line:  %d\n", exromLine);

    if (cartridge == NULL) {
        msg("No cartridge attached\n");
    } else {
        cartridge->dump();
    }
}

CartridgeType
ExpansionPort::getCartridgeType()
{
    return cartridge ? cartridge->getCartridgeType() : CRT_NONE;
}

u8
ExpansionPort::peek(u16 addr)
{
    return cartridge ? cartridge->peek(addr) : 0;
}

u8
ExpansionPort::spypeek(u16 addr)
{
    return cartridge ? cartridge->spypeek(addr) : 0;
}

u8
ExpansionPort::peekIO1(u16 addr)
{
    /* "Die beiden mit "I/O 1" und "I/O 2" bezeichneten Bereiche
     *  sind für Erweiterungskarten reserviert und normalerweise ebenfalls offen,
     *  ein Lesezugriff liefert auch hier "zufällige" Daten (dass diese Daten gar
     *  nicht so zufällig sind, wird in Kapitel 4 noch ausführlich erklärt. Ein
     *  Lesen von offenen Adressen liefert nämlich auf vielen C64 das zuletzt vom
     *  VIC gelesene Byte zurück!)" [C.B.]
     */
    return cartridge ? cartridge->peekIO1(addr) : vic.getDataBusPhi1();
}

u8
ExpansionPort::spypeekIO1(u16 addr)
{
    return cartridge ? cartridge->spypeekIO1(addr) : vic.getDataBusPhi1();
}

u8
ExpansionPort::peekIO2(u16 addr)
{
    return cartridge ? cartridge->peekIO2(addr) : vic.getDataBusPhi1();
}

u8
ExpansionPort::spypeekIO2(u16 addr)
{
    return cartridge ? cartridge->spypeekIO2(addr) : vic.getDataBusPhi1();
}

void
ExpansionPort::poke(u16 addr, u8 value)
{
    if (cartridge) {
        cartridge->poke(addr, value);
    } else if (!c64.getUltimax()) {
        mem.ram[addr] = value;
    }
}

void
ExpansionPort::pokeIO1(u16 addr, u8 value)
{
    assert(addr >= 0xDE00 && addr <= 0xDEFF);
    
    if (cartridge) cartridge->pokeIO1(addr, value);
}

void
ExpansionPort::pokeIO2(u16 addr, u8 value)
{
    assert(addr >= 0xDF00 && addr <= 0xDFFF);
    
    if (cartridge) cartridge->pokeIO2(addr, value);
}

void
ExpansionPort::setGameLine(bool value)
{
    gameLine = value;
    vic.setUltimax(!gameLine && exromLine);
    mem.updatePeekPokeLookupTables();
}

void
ExpansionPort::setExromLine(bool value)
{
    exromLine = value;
    vic.setUltimax(!gameLine && exromLine);
    mem.updatePeekPokeLookupTables();
}

void
ExpansionPort::setGameAndExrom(bool game, bool exrom)
{
    gameLine = game;
    exromLine = exrom;
    vic.setUltimax(!gameLine && exromLine);
    mem.updatePeekPokeLookupTables();
}

CRTMode
ExpansionPort::getCartridgeMode()
{
    switch ((exromLine ? 0b10 : 0) | (gameLine ? 0b01 : 0)) {
            
        case 0b00: return CRTMODE_16K;
        case 0b01: return CRTMODE_8K;
        case 0b10: return CRTMODE_ULTIMAX;
        default:   return CRTMODE_OFF;
    }
}

void
ExpansionPort::setCartridgeMode(CRTMode mode)
{
    switch (mode) {
        case CRTMODE_16K:     setGameAndExrom(0,0); return;
        case CRTMODE_8K:      setGameAndExrom(1,0); return;
        case CRTMODE_ULTIMAX: setGameAndExrom(0,1); return;
        default:          setGameAndExrom(1,1);
    }
}

void
ExpansionPort::attachCartridge(Cartridge *c)
{
    assert(c);
    
    // Remove old cartridge (if any) and assign new one
    detachCartridge();
    cartridge = c;
    crtType = c->getCartridgeType();
    
    // Reset cartridge to update exrom and game line on the expansion port
    cartridge->reset();
    
    c64.putMessage(MSG_CARTRIDGE);
    if (cartridge->hasSwitch()) c64.putMessage(MSG_CART_SWITCH);
    
    trace(EXP_DEBUG, "Cartridge attached to expansion port");
}

bool
ExpansionPort::attachCartridgeAndReset(CRTFile *file)
{
    assert(file);
    
    Cartridge *cartridge = Cartridge::makeWithCRTFile(c64, file);
    
    if (cartridge) {
        
        suspend();
        attachCartridge(cartridge);
        c64.reset();
        resume();
        return true;
    }
    
    return false;
}

void
ExpansionPort::attachGeoRamCartridge(usize kb)
{
    trace(EXP_DEBUG, "Attaching GeoRAM cartridge (%zu KB)", kb);

    if (kb != 64 && kb != 256 && kb != 512 && kb != 1024 && kb != 2048) {
        assert(false);
    }
    
    Cartridge *geoRAM = Cartridge::makeWithType(c64, CRT_GEO_RAM);
    geoRAM->setRamCapacity(kb * 1024);
    attachCartridge(geoRAM);
}

void
ExpansionPort::attachIsepicCartridge()
{
    trace(EXP_DEBUG, "Attaching Isepic cartridge\n");
    
    Cartridge *isepic = Cartridge::makeWithType(c64, CRT_ISEPIC);
    return attachCartridge(isepic);
}

void
ExpansionPort::detachCartridge()
{
    if (cartridge) {
        
        suspend();
        
        delete cartridge;
        cartridge = NULL;
        crtType = CRT_NONE;
        
        setCartridgeMode(CRTMODE_OFF);
        
        trace(EXP_DEBUG, "Cartridge detached from expansion port");
        c64.putMessage(MSG_NO_CARTRIDGE);
       
        resume();
    }
}

void
ExpansionPort::detachCartridgeAndReset()
{
    suspend();
    detachCartridge();
    c64.reset();
    resume();
}

bool
ExpansionPort::hasBattery()
{
    return cartridge ? cartridge->getBattery() : false;
}

void
ExpansionPort::setBattery(bool value)
{
    if (cartridge) cartridge->setBattery(value);
}

long
ExpansionPort::numButtons()
{
    return cartridge ? cartridge->numButtons() : 0;
}

const char *
ExpansionPort::getButtonTitle(unsigned nr)
{
     return cartridge ? cartridge->getButtonTitle(nr) : NULL;
}
 
void
ExpansionPort::pressButton(unsigned nr)
{
    if (cartridge) cartridge->pressButton(nr);
}

void
ExpansionPort::releaseButton(unsigned nr)
{
    if (cartridge) cartridge->releaseButton(nr);
}

bool
ExpansionPort::hasSwitch()
{
    return cartridge ? cartridge->hasSwitch() : false;
}

i8
ExpansionPort::getSwitch()
{
    return cartridge ? cartridge->getSwitch() : 0;
}

bool
ExpansionPort::switchIsNeutral()
{
    return cartridge ? cartridge->switchIsNeutral() : false;
}
   
bool
ExpansionPort::switchIsLeft()
{
    return cartridge ? cartridge->switchIsLeft() : false;
}
   
bool
ExpansionPort::switchIsRight()
{
    return cartridge ? cartridge->switchIsRight() : false;
}

const char *
ExpansionPort::getSwitchDescription(i8 pos)
{
    return cartridge ? cartridge->getSwitchDescription(pos) : NULL;
}

const char *
ExpansionPort::getSwitchDescription()
{
    return getSwitchDescription(getSwitch());
}

bool
ExpansionPort::validSwitchPosition(i8 pos)
{
    return cartridge ? cartridge->validSwitchPosition(pos) : false;    
}

bool
ExpansionPort::hasLED()
{
    return cartridge ? cartridge->hasLED() : false;
}
 
bool
ExpansionPort::getLED()
{
    return cartridge ? cartridge->getLED() : false;
}
 
void
ExpansionPort::setLED(bool value)
{
    if (cartridge) cartridge->setLED(value);
}

void
ExpansionPort::execute()
{
    if (cartridge) cartridge->execute();
}

void
ExpansionPort::updatePeekPokeLookupTables()
{
    if (cartridge) cartridge->updatePeekPokeLookupTables();
}

void
ExpansionPort::nmiWillTrigger()
{
    if (cartridge) cartridge->nmiWillTrigger();
}

void
ExpansionPort::nmiDidTrigger()
{
    if (cartridge) cartridge->nmiDidTrigger();    
}
