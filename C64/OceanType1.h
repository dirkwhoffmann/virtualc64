/*!
 * @header      OceanType1.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   2018 Dirk W. Hoffmann
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

#ifndef _OCEAN_TYPE1_INC
#define _OCEAN_TYPE1_INC

#include "Cartridge.h"

/*!
 * @brief    Custom implementation of Ocean type cartridges
 * @details  Cartridges of this type comprise Navy Seals, Robocop 2 & 3,
 *           Shadow  of the Beast, Toki, Terminator 2 and more.
 */
class OceanType1 : public Cartridge {
    
public:
    
    using Cartridge::Cartridge;

    CartridgeType getCartridgeType() { return CRT_OCEAN_TYPE_1; }
    void poke(uint16_t addr, uint8_t value);
};

#endif

