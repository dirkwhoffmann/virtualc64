/*!
 * @header      SimonsBasic.h
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

#if 0

#ifndef _SIMONS_BASIC
#define _SIMONS_BASIC

#include "Cartridge.h"

/*!
 * @brief    Custom implementation of Simons Basic cartridge
 */
class SimonsBasic : public Cartridge {
    
public:

    using Cartridge::Cartridge;

    CartridgeType getCartridgeType() { return CRT_SIMONS_BASIC; }
    void powerup();
    uint8_t peekIO(uint16_t addr);
    void poke(uint16_t addr, uint8_t value);
};

#endif

#endif

