// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _EASYFLASH_INC
#define _EASYFLASH_INC

#include "Cartridge.h"

class EasyFlash : public Cartridge {
    
    //!@brief    Flash Rom mapping to ROML ($8000 - $9FFF)
    FlashRom flashRomL;

    //!@brief    Flash Rom mapping to ROMH ($A000 - $B000 or $E000 - $FFFF)
    FlashRom flashRomH;
    
    //!@brief    Selected memory bank
    uint8_t bank;
    
    //!@brief    The jumper
    bool jumper;

public:
    
    //
    //! @functiongroup Creating and destructing
    //
    
    EasyFlash(C64 *c64);
    CartridgeType getCartridgeType() { return CRT_EASYFLASH; }
    

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
    
    void resetCartConfig();
    void loadChip(unsigned nr, CRTFile *c);
    uint8_t peek(u16 addr);
    void poke(u16 addr, uint8_t value);
    uint8_t peekIO1(u16 addr);
    uint8_t peekIO2(u16 addr);
    void pokeIO1(u16 addr, uint8_t value);
    void pokeIO2(u16 addr, uint8_t value);
    bool hasLED() { return true; }
};

#endif
