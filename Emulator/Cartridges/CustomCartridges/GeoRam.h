// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _GEORAM_INC
#define _GEORAM_INC

#include "Cartridge.h"

class GeoRAM : public Cartridge {
    
private:
    
    //! @brief   Selected RAM bank
    u8 bank;
    
    //! @brief   Selected page inside the selected RAM bank.
    u8 page;
    
    //! @brief   Computes the offset for accessing the cartridge RAM
    unsigned offset(u8 addr);
    
public:
    GeoRAM(C64 *c64, C64 &ref);
    CartridgeType getCartridgeType() { return CRT_GEO_RAM; }
    void reset();
    size_t stateSize();
    void didLoadFromBuffer(u8 **buffer);
    void didSaveToBuffer(u8 **buffer);
    u8 peekIO1(u16 addr);
    u8 peekIO2(u16 addr);
    void pokeIO1(u16 addr, u8 value);
    void pokeIO2(u16 addr, u8 value);
};

#endif
