/*!
 * @header      FinalIII.h
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

#ifndef _FINAL_III
#define _FINAL_III

#include "Cartridge.h"

// TODO:
// 1. Implement peekIO correctly
// 2. 
/*!
 * @brief    Custom implementation of Final Cartridge III
 */
class FinalIII : public Cartridge {
    
public:
    
    using Cartridge::Cartridge;
    
    CartridgeType getCartridgeType() { return CRT_FINAL_CARTRIDGE_III; }
    void powerup();
    uint8_t peekIO(uint16_t addr);
    void poke(uint16_t addr, uint8_t value);
    
    //! @brief   Virtual push of the cartriges's reset button
    void pushReset();

    //! @brief   Virtual push of the cartriges's freeze button
    void pushFreeze();

    //! @brief   Triggers an NMI interrupt
    void triggerNMI();
    
};

#endif

