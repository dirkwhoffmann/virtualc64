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
    uint8_t bank;
    
    //! @brief   Selected page inside the selected RAM bank.
    uint8_t page;
    
    //! @brief   Computes the offset for accessing the cartridge RAM
    unsigned offset(uint8_t addr);
    
public:
    GeoRAM(C64 *c64);
    CartridgeType getCartridgeType() { return CRT_GEO_RAM; }
    void reset();
    size_t stateSize();
    void didLoadFromBuffer(uint8_t **buffer);
    void didSaveToBuffer(uint8_t **buffer);
    uint8_t peekIO1(uint16_t addr);
    uint8_t peekIO2(uint16_t addr);
    void pokeIO1(uint16_t addr, uint8_t value);
    void pokeIO2(uint16_t addr, uint8_t value);
};

#endif
