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

/*! @brief    This class implements a single Rom packet inside an expansion
 *            port cartridge.
 */
class CartridgeRom : public VirtualComponent {
    
    //! @brief    Rom size in bytes
    uint16_t size;
    
    //! @brief    Rom data
    uint8_t *rom;
    
    public:
    
    //! @brief    Constructor
    CartridgeRom(uint16_t sizeInBytes);
    CartridgeRom(uint16_t sizeInBytes, const uint8_t *buffer);
    
    //! @brief    Destructor
    ~CartridgeRom();
    
    //! @brief    Methods from VirtualComponent
    size_t stateSize();
    void loadFromBuffer(uint8_t **buffer);
    void saveToBuffer(uint8_t **buffer);
    
    //! @brief    Reads a ROM cell
    uint8_t peek(uint16_t addr);
    
    //! @brief    Reads a ROM cell without side effects
    uint8_t spypeek(uint16_t addr) { return peek(addr); }
    
    //! @brief    Writes a ROM cell
    void poke(uint16_t addr, uint8_t value) { }
    
};

#endif


