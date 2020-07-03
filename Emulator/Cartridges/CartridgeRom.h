// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _CARTRIDGEROM_INC
#define _CARTRIDGEROM_INC

#include "HardwareComponent.h"

/*! @brief    This class implements a cartridge Rom chip 
 */
class CartridgeRom : public HardwareComponent {
    
    protected:
    
    //! @brief    Rom data
    uint8_t *rom = NULL;
    
    public:
    
    //! @brief    Size in bytes
    u16 size = 0;
    
    /*! @brief    Load address
     *  @details  This value is taken from the .CRT file. Possible values are
     *            $8000 for chips mapping into the ROML area, $A000 for chips
     *            mapping into the ROMH area in 16KB game mode, and $E000 for
     *            chips mapping into the ROMH area in ultimax mode.
     */
    u16 loadAddress = 0;
    
    public:
    
    //! @brief    Constructor
    CartridgeRom();
    // CartridgeRom(uint8_t **buffer);
    CartridgeRom(u16 _size, u16 _loadAddress, const uint8_t *buffer = NULL);
    
    //! @brief    Destructor
    ~CartridgeRom();
    
    //! @brief    Methods from HardwareComponent
    size_t stateSize();
    void didLoadFromBuffer(uint8_t **buffer);
    void didSaveToBuffer(uint8_t **buffer);
    
    //! @brief    Returns true if this Rom chip maps to ROML, only.
    bool mapsToL();
    
    //! @brief    Returns true if this Rom chip maps to ROML and ROMH.
    bool mapsToLH();
    
    //! @brief    Returns true if this Rom chip maps to ROMH, only.
    bool mapsToH();
    
    //! @brief    Reads a ROM cell
    uint8_t peek(u16 addr);
    
    //! @brief    Reads a ROM cell without side effects
    uint8_t spypeek(u16 addr) { return peek(addr); }
    
    //! @brief    Writes a ROM cell
    void poke(u16 addr, uint8_t value) { }
    
};

#endif


