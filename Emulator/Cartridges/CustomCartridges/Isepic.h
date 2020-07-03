// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _ISEPIC_INC
#define _ISEPIC_INC

#include "Cartridge.h"

class Isepic : public Cartridge {
    
    //! @brief   Selected page inside the selected RAM bank.
    uint8_t page;

    //! @brief   Original mapping of the uppermost memory page
    MemoryType oldPeekSource;
    MemoryType oldPokeTarget;

public:
    Isepic(C64 *c64);
    CartridgeType getCartridgeType() override { return CRT_ISEPIC; }
    
    void reset() override;
    size_t stateSize() override;
    void didLoadFromBuffer(uint8_t **buffer) override;
    void didSaveToBuffer(uint8_t **buffer) override;
    
    bool hasSwitch() override { return true; }
    const char *getSwitchDescription(int8_t pos) override;
    void setSwitch(int8_t pos) override;
    bool switchInOffPosition() { return switchIsLeft(); }
    bool switchInOnPosition() { return switchIsRight(); }

    bool cartIsVisible() { return switchInOnPosition(); }
    bool cartIsHidden() { return !cartIsVisible(); }

    void updatePeekPokeLookupTables() override;
    uint8_t peek(uint16_t addr) override;
    uint8_t peekIO1(uint16_t addr) override;
    uint8_t peekIO2(uint16_t addr) override;
    void poke(uint16_t addr, uint8_t value) override;
    void pokeIO1(uint16_t addr, uint8_t value) override;
    void pokeIO2(uint16_t addr, uint8_t value) override;
};


#endif
