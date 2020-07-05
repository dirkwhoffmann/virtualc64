// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _CARTRIDGEROM_H
#define _CARTRIDGEROM_H

#include "C64Component.h"

/*! @brief    This class implements a cartridge Rom chip 
 */
class CartridgeRom : public C64Component {
    
    protected:
    
    //! @brief    Rom data
    u8 *rom = NULL;
    
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
    
    
    //
    // Constructing and serializing
    //
    
public:
    
    CartridgeRom(C64 &ref);
    CartridgeRom(C64 &ref, u16 _size, u16 _loadAddress, const u8 *buffer = NULL);
    ~CartridgeRom();
    
    
    //
    // Methods from HardwareComponent
    //
    
public:
    
    size_t stateSize() override;
    void didLoadFromBuffer(u8 **buffer) override;
    void didSaveToBuffer(u8 **buffer) override;
    
    
public:
    
    //! @brief    Returns true if this Rom chip maps to ROML, only.
    bool mapsToL();
    
    //! @brief    Returns true if this Rom chip maps to ROML and ROMH.
    bool mapsToLH();
    
    //! @brief    Returns true if this Rom chip maps to ROMH, only.
    bool mapsToH();
    
    //! @brief    Reads a ROM cell
    u8 peek(u16 addr);
    
    //! @brief    Reads a ROM cell without side effects
    u8 spypeek(u16 addr) { return peek(addr); }
    
    //! @brief    Writes a ROM cell
    void poke(u16 addr, u8 value) { }
    
};

#endif


