// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"

ExpansionPort::ExpansionPort(C64 &ref) : C64Component(ref)
{
    setDescription("Expansion port");
    
    // Register snapshot items
    SnapshotItem items[] = {
        
        // Internal state
        { &gameLine,          sizeof(gameLine),        KEEP_ON_RESET },
        { &exromLine,         sizeof(exromLine),       KEEP_ON_RESET },
        { NULL,               0,                       0 }};
    
    registerSnapshotItems(items, sizeof(items));
}

ExpansionPort::~ExpansionPort()
{
    detachCartridge();
}

void
ExpansionPort::_reset()
{
    // Clear snapshot items marked with 'CLEAR_ON_RESET'
     if (snapshotItems != NULL)
         for (unsigned i = 0; snapshotItems[i].data != NULL; i++)
             if (snapshotItems[i].flags & CLEAR_ON_RESET)
                 memset(snapshotItems[i].data, 0, snapshotItems[i].size);

    if (cartridge) {
        cartridge->reset();
        cartridge->resetCartConfig();
    } else {
        setCartridgeMode(CRT_OFF);
    }
}

void
ExpansionPort::_ping()
{
    c64.putMessage(cartridge ? MSG_CARTRIDGE : MSG_NO_CARTRIDGE);
    c64.putMessage(MSG_CART_SWITCH);
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
    
    debug(SNP_DEBUG, "Recreated from %d bytes\n", reader.ptr - buffer);
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
    
    debug(SNP_DEBUG, "Serialized to %d bytes\n", writer.ptr - buffer); \
    return writer.ptr - buffer;
}

size_t
ExpansionPort::oldStateSize()
{
    return HardwareComponent::oldStateSize()
    + 2
    + (cartridge ? cartridge->oldStateSize() : 0);
}

void
ExpansionPort::oldDidLoadFromBuffer(u8 **buffer)
{
    // Delete old cartridge (if any)
    if (cartridge != NULL) {
        delete cartridge;
        cartridge = NULL;
    }
    
    // Read cartridge type and cartridge (if any)
    CartridgeType cartridgeType = (CartridgeType)read16(buffer);
    if (cartridgeType != CRT_NONE) {
        cartridge = Cartridge::makeWithType(c64, cartridgeType);
        cartridge->oldLoadFromBuffer(buffer);
    }
}

void
ExpansionPort::oldDidSaveToBuffer(u8 **buffer)
{
    // Write cartridge type and data (if any)
    write16(buffer, cartridge ? cartridge->getCartridgeType() : CRT_NONE);
    if (cartridge != NULL)
        cartridge->oldSaveToBuffer(buffer);
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

/*
u8
ExpansionPort::peek(u16 addr)
{
    assert((addr >= 0x8000 && addr <= 0x9FFF) ||
           (addr >= 0xA000 && addr <= 0xBFFF) ||
           (addr >= 0xE000 && addr <= 0xFFFF));
    
    if (cartridge) {
        if (addr <= 0x9FFF) {
            return cartridge->peekRomLabs(addr);
        } else {
            return cartridge->peekRomHabs(addr);
        }
    }
    return 0;
}
*/

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

CartridgeMode
ExpansionPort::getCartridgeMode()
{
    switch ((exromLine ? 0b10 : 0) | (gameLine ? 0b01 : 0)) {
            
        case 0b00: return CRT_16K;
        case 0b01: return CRT_8K;
        case 0b10: return CRT_ULTIMAX;
        default:   return CRT_OFF;
    }
}

void
ExpansionPort::setCartridgeMode(CartridgeMode mode)
{
    switch (mode) {
        case CRT_16K:     setGameAndExrom(0,0); return;
        case CRT_8K:      setGameAndExrom(1,0); return;
        case CRT_ULTIMAX: setGameAndExrom(0,1); return;
        default:          setGameAndExrom(1,1);
    }
}

void
ExpansionPort::attachCartridge(Cartridge *c)
{
    assert(c != NULL);
    
    // Remove old cartridge (if any) and assign new one
    detachCartridge();
    cartridge = c;
    crtType = c->getCartridgeType();
    
    // Reset cartridge to update exrom and game line on the expansion port
    cartridge->reset();
    
    c64.putMessage(MSG_CARTRIDGE);
    if (cartridge->hasSwitch()) c64.putMessage(MSG_CART_SWITCH);
    
    debug(EXP_DEBUG, "Cartridge attached to expansion port");
}

bool
ExpansionPort::attachCartridgeAndReset(CRTFile *file)
{
    assert(file != NULL);
    
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

bool
ExpansionPort::attachGeoRamCartridge(u32 capacity)
{
    debug(EXP_DEBUG, "Attaching GeoRAM cartridge (%d KB)\n", capacity);

    switch (capacity) {
        case 64: case 128: case 256: case 512: case 1024: case 2048: case 4096:
            break;
        default:
            warn("Cannot create GeoRAM cartridge of size %d\n", capacity);
            return false;
    }
    
    Cartridge *geoRAM = Cartridge::makeWithType(c64, CRT_GEO_RAM);
    u32 capacityInBytes = capacity * 1024;
    geoRAM->setRamCapacity(capacityInBytes);
    
    attachCartridge(geoRAM);
    return true;
}

void
ExpansionPort::attachIsepicCartridge()
{
    debug(EXP_DEBUG, "Attaching Isepic cartridge\n");
    
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
        
        setCartridgeMode(CRT_OFF);
        
        debug(EXP_DEBUG, "Cartridge detached from expansion port");
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

unsigned
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
