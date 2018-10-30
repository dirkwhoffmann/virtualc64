/*!
 * @header      CartridgeRom.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   Dirk W. Hoffmann, all rights reserved.
 */
/* This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _CARTRIDGEROM_INC
#define _CARTRIDGEROM_INC

#include "VirtualComponent.h"

/*! @brief    This class implements a cartridge Rom chip 
 */
class CartridgeRom : public VirtualComponent {
    
    protected:
    
    //! @brief    Rom data
    uint8_t *rom;
    
    public:
    
    //! @brief    Size in bytes
    uint16_t size;
    
    /*! @brief    Load address
     *  @details  This value is taken from the .CRT file. Possible values are
     *            $8000 for chips mapping into the ROML area, $A000 for chips
     *            mapping into the ROMH area in 16KB game mode, and $E000 for
     *            chips mapping into the ROMH area in ultimax mode.
     */
    uint16_t loadAddress;
    
    public:
    
    //! @brief    Constructor
    CartridgeRom(uint8_t **buffer);
    CartridgeRom(uint16_t _size, uint16_t _loadAddress, const uint8_t *buffer = NULL);
    
    //! @brief    Destructor
    ~CartridgeRom();
    
    //! @brief    Methods from VirtualComponent
    size_t stateSize();
    void loadFromBuffer(uint8_t **buffer);
    void saveToBuffer(uint8_t **buffer);
    
    //! @brief    Returns true if this Rom chip maps to ROML, only.
    bool mapsToL();
    
    //! @brief    Returns true if this Rom chip maps to ROML and ROMH.
    bool mapsToLH();
    
    //! @brief    Returns true if this Rom chip maps to ROMH, only.
    bool mapsToH();
    
    //! @brief    Reads a ROM cell
    uint8_t peek(uint16_t addr);
    
    //! @brief    Reads a ROM cell without side effects
    uint8_t spypeek(uint16_t addr) { return peek(addr); }
    
    //! @brief    Writes a ROM cell
    void poke(uint16_t addr, uint8_t value) { }
    
};

#endif


