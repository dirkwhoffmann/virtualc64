// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _EXPERT_INC
#define _EXPERT_INC

#include "Cartridge.h"

class Expert : public Cartridge {
    
    // On-board flipflop
    bool active;
    
public:
    
    Expert(C64 *c64);
    CartridgeType getCartridgeType() { return CRT_EXPERT; }
    
    //
    //! @functiongroup Methods from HardwareComponent
    //
    
    void reset();
    void dump();
    size_t stateSize();
    void didLoadFromBuffer(uint8_t **buffer);
    void didSaveToBuffer(uint8_t **buffer);
    
    //
    //! @functiongroup Methods from Cartridge
    //
    
    void loadChip(unsigned nr, CRTFile *c);
    
    unsigned numButtons() { return 2; }
    const char *getButtonTitle(unsigned nr);
    void pressButton(unsigned nr);
    
    bool hasSwitch() { return true; }
    const char *getSwitchDescription(int8_t pos);
    bool switchInPrgPosition() { return switchIsLeft(); }
    bool switchInOffPosition() { return switchIsNeutral(); }
    bool switchInOnPosition() { return switchIsRight(); }

    void updatePeekPokeLookupTables();
    uint8_t peek(uint16_t addr);
    uint8_t peekIO1(uint16_t addr);
    uint8_t spypeekIO1(uint16_t addr) { return 0; }
    void poke(uint16_t addr, uint8_t value);
    void pokeIO1(uint16_t addr, uint8_t value);
    
    void nmiWillTrigger();
    
    //! @brief    Returns true if cartridge RAM is visible
    bool cartridgeRamIsVisible(uint16_t addr);    

    //! @brief    Returns true if cartridge RAM is write enabled
    bool cartridgeRamIsWritable(uint16_t addr);
};


#endif
