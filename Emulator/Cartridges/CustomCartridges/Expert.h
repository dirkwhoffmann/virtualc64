// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _EXPERT_H
#define _EXPERT_H

#include "Cartridge.h"

class Expert : public Cartridge {
    
    // On-board flipflop
    bool active;
    
public:
    
    Expert(C64 *c64, C64 &ref);
    CartridgeType getCartridgeType() override { return CRT_EXPERT; }
    
    
    //
    // Methods from HardwareComponent
    //
    
private:
        
    void oldReset() override;
    void _dump() override;
    size_t stateSize() override;
    void didLoadFromBuffer(u8 **buffer) override;
    void didSaveToBuffer(u8 **buffer) override;
    
    
    //
    // Methods from Cartridge
    //
    
    void loadChip(unsigned nr, CRTFile *c) override;
    
    unsigned numButtons() override { return 2; }
    const char *getButtonTitle(unsigned nr) override;
    void pressButton(unsigned nr) override;
    
    bool hasSwitch() override { return true; }
    const char *getSwitchDescription(i8 pos) override;
    bool switchInPrgPosition() { return switchIsLeft(); }
    bool switchInOffPosition() { return switchIsNeutral(); }
    bool switchInOnPosition() { return switchIsRight(); }

    void updatePeekPokeLookupTables() override;
    u8 peek(u16 addr) override;
    u8 peekIO1(u16 addr) override;
    u8 spypeekIO1(u16 addr) override { return 0; }
    void poke(u16 addr, u8 value) override;
    void pokeIO1(u16 addr, u8 value) override;
    
    void nmiWillTrigger() override;
    
    // Returns true if cartridge RAM is visible
    bool cartridgeRamIsVisible(u16 addr);    

    // Returns true if cartridge RAM is write enabled
    bool cartridgeRamIsWritable(u16 addr);
};

#endif
