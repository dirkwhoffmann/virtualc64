// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _EASYFLASH_H
#define _EASYFLASH_H

#include "Cartridge.h"

class EasyFlash : public Cartridge {
    
    //!@brief    Flash Rom mapping to ROML ($8000 - $9FFF)
    FlashRom flashRomL = FlashRom(vc64);

    //!@brief    Flash Rom mapping to ROMH ($A000 - $B000 or $E000 - $FFFF)
    FlashRom flashRomH = FlashRom(vc64);
    
    //!@brief    Selected memory bank
    u8 bank;
    
    //!@brief    The jumper
    bool jumper;

public:
    
    //
    //! @functiongroup Creating and destructing
    //
    
    EasyFlash(C64 *c64, C64 &ref);
    CartridgeType getCartridgeType() { return CRT_EASYFLASH; }
    

    //
    //! @functiongroup Methods from HardwareComponent
    //

    void reset();
    void dump();
    size_t stateSize();
    void didLoadFromBuffer(u8 **buffer);
    void didSaveToBuffer(u8 **buffer);
    
    
    //
    //! @functiongroup Methods from Cartridge
    //
    
    void resetCartConfig();
    void loadChip(unsigned nr, CRTFile *c);
    u8 peek(u16 addr);
    void poke(u16 addr, u8 value);
    u8 peekIO1(u16 addr);
    u8 peekIO2(u16 addr);
    void pokeIO1(u16 addr, u8 value);
    void pokeIO2(u16 addr, u8 value);
    bool hasLED() { return true; }
};

#endif
