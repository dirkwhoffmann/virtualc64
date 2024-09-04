// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// This FILE is dual-licensed. You are free to choose between:
//
//     - The GNU General Public License v3 (or any later version)
//     - The Mozilla Public License v2
//
// SPDX-License-Identifier: GPL-3.0-or-later OR MPL-2.0
// -----------------------------------------------------------------------------

#include "config.h"
#include "ExpansionPort.h"
#include "C64.h"

namespace vc64 {

const CartridgeTraits &
ExpansionPort::getCartridgeTraits() const
{
    static CartridgeTraits none = CartridgeTraits { .type = CRT_NONE };

    return cartridge ? cartridge->getCartridgeTraits() : none;
}

const CartridgeInfo &
ExpansionPort::getInfo() const
{
    static CartridgeInfo dummy = { };
    
    return cartridge ? cartridge->getInfo() : dummy;
}

CartridgeRomInfo
ExpansionPort::getRomInfo(isize nr) const
{
    return cartridge ? cartridge->getRomInfo(nr) : CartridgeRomInfo { };
}

CartridgeType
ExpansionPort::getCartridgeType() const
{
    return cartridge ? cartridge->getCartridgeType() : CRT_NONE;
}

u8
ExpansionPort::peek(u16 addr)
{
    return cartridge ? cartridge->peek(addr) : 0;
}

u8
ExpansionPort::spypeek(u16 addr) const
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
ExpansionPort::spypeekIO1(u16 addr) const
{
    return cartridge ? cartridge->spypeekIO1(addr) : vic.getDataBusPhi1();
}

u8
ExpansionPort::peekIO2(u16 addr)
{
    return cartridge ? cartridge->peekIO2(addr) : vic.getDataBusPhi1();
}

u8
ExpansionPort::spypeekIO2(u16 addr) const
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
ExpansionPort::getCartridgeMode() const
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
        default:              setGameAndExrom(1,1);
    }
}

void
ExpansionPort::resetCartConfig()
{
    cartridge ? cartridge->resetCartConfig() : setCartridgeMode(CRTMODE_OFF);
}

void
ExpansionPort::attachCartridge(Cartridge *c)
{
    assert(c);
    assert(c->isSupported());
    
    {   SUSPENDED
        
        // Remove old cartridge (if any) and assign new one
        detachCartridge();
        cartridge = std::unique_ptr<Cartridge>(c);
        crtType = c->getCartridgeType();
        
        // Reset cartridge to update exrom and game line on the expansion port
        cartridge->hardReset();
        
        msgQueue.put(MSG_CRT_ATTACHED, 1);
        debug(EXP_DEBUG, "Cartridge attached to expansion port");
        
    }
}

void
ExpansionPort::attachReu(isize kb)
{
    debug(EXP_DEBUG, "Attaching REU (%ld KB)\n", kb);
    attachCartridge(new Reu(c64, kb));
}

void
ExpansionPort::attachGeoRam(isize kb)
{
    debug(EXP_DEBUG, "Attaching GeoRAM (%ld KB)\n", kb);
    attachCartridge(new GeoRAM(c64, kb));
}

void
ExpansionPort::attachCartridge(const fs::path &path, bool reset)
{
    attachCartridge(CRTFile(path), reset);
}

void
ExpansionPort::attachCartridge(const MediaFile &file, bool reset)
{
    try {

        const CRTFile &crtFile = dynamic_cast<const CRTFile &>(file);

        // Only proceed if this cartridge is supported
        if (!crtFile.isSupported()) {
            throw Error(VC64ERROR_CRT_UNSUPPORTED, crtFile.cartridgeTypeName());
        }

        // Create cartridge from cartridge file
        Cartridge *cartridge = Cartridge::makeWithCRTFile(c64, crtFile);

        // Attach cartridge to the expansion port
        {   SUSPENDED

            attachCartridge(cartridge);
            if (reset) c64.hardReset();
        }

    } catch (...) {

        throw Error(VC64ERROR_FILE_TYPE_MISMATCH);
    }
}

void
ExpansionPort::attachIsepicCartridge()
{
    debug(EXP_DEBUG, "Attaching Isepic cartridge\n");
    
    Cartridge *isepic = new Isepic(c64); //  Cartridge::makeWithType(c64, CRT_ISEPIC);
    (void)attachCartridge(isepic);
}

void
ExpansionPort::detachCartridge()
{
    {   SUSPENDED
        
        if (cartridge) {
            
            cartridge = nullptr;
            crtType = CRT_NONE;
            
            setCartridgeMode(CRTMODE_OFF);
            
            debug(EXP_DEBUG, "Cartridge detached from expansion port");
            msgQueue.put(MSG_CRT_ATTACHED, 0);
        }
    }
}

void
ExpansionPort::pressButton(isize nr)
{
    assert(nr == 1 || nr == 2);
    if (cartridge) cartridge->pressButton(nr);
}

void
ExpansionPort::releaseButton(isize nr)
{
    assert(nr == 1 || nr == 2);
    if (cartridge) cartridge->releaseButton(nr);
}

void
ExpansionPort::setLED(bool value)
{
    if (cartridge) cartridge->setLED(value);
}

void
ExpansionPort::processCommand(const Cmd &cmd)
{
    switch (cmd.type) {

        case CMD_CRT_BUTTON_PRESS:      pressButton(isize(cmd.value)); break;
        case CMD_CRT_BUTTON_RELEASE:    releaseButton(isize(cmd.value)); break;
        case CMD_CRT_SWITCH_LEFT:       setSwitch(-1); break;
        case CMD_CRT_SWITCH_NEUTRAL:    setSwitch(0); break;
        case CMD_CRT_SWITCH_RIGHT:      setSwitch(1); break;

        default:
            fatalError;
    }
}

void 
ExpansionPort::processEvent(EventID id)
{
    if (cartridge) {
        cartridge->processEvent(id);
    } else {
        c64.cancel<SLOT_EXP>();
    }
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

}
