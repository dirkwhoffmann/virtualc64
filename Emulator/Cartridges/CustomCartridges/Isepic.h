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
    u8 page;

    //! @brief   Original mapping of the uppermost memory page
    MemoryType oldPeekSource;
    MemoryType oldPokeTarget;

public:
    Isepic(C64 *c64);
    CartridgeType getCartridgeType() override { return CRT_ISEPIC; }
    
    void reset() override;
    size_t stateSize() override;
    void didLoadFromBuffer(u8 **buffer) override;
    void didSaveToBuffer(u8 **buffer) override;
    
    bool hasSwitch() override { return true; }
    const char *getSwitchDescription(i8 pos) override;
    void setSwitch(i8 pos) override;
    bool switchInOffPosition() { return switchIsLeft(); }
    bool switchInOnPosition() { return switchIsRight(); }

    bool cartIsVisible() { return switchInOnPosition(); }
    bool cartIsHidden() { return !cartIsVisible(); }

    void updatePeekPokeLookupTables() override;
    u8 peek(u16 addr) override;
    u8 peekIO1(u16 addr) override;
    u8 peekIO2(u16 addr) override;
    void poke(u16 addr, u8 value) override;
    void pokeIO1(u16 addr, u8 value) override;
    void pokeIO2(u16 addr, u8 value) override;
};


#endif
