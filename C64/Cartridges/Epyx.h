/*!
 * @header      Epyx.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   Dirk W. Hoffmann. All rights reserved.
 */
/*
 * This program is free software; you can redistribute it and/or modify
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

#ifndef _EPYX_INC
#define _EPYX_INC

#include "Cartridge.h"

class EpyxFastLoad : public Cartridge {
    
    private:
    
    //! @brief    Discharges the cartridge's capacitor
    /*! @details  The Epyx cartridge utilizes a capacitor to switch the ROM on and off.
     *            During normal operation, the capacitor slowly charges. When it is
     *            completely charged, the ROM gets disabled. When the cartridge is attached,
     *            the capacitor is discharged and the ROM visible. To avoid the ROM to be
     *            disabled, the cartridge can either read from ROML or I/O space 1. Both
     *            operations discharge the capacitor and keep the ROM alive.
     */
    void dischargeCapacitor();
    
    public:
    
    //! @brief    Checks the capacitor and switched off cartridge if required
    /*! @return   true if the cartridge is active, and false if the cartridge is disabled.
     */
    bool checkCapacitor();
    
    
    public:
    using Cartridge::Cartridge;
    CartridgeType getCartridgeType() { return CRT_EPYX_FASTLOAD; }
    void reset();
    void resetCartConfig();
    void execute();
    uint8_t peekRomL(uint16_t addr);
    uint8_t peekRomH(uint16_t addr);
    uint8_t spypeekRomL(uint16_t addr) { return Cartridge::peekRomL(addr); }
    uint8_t spypeekRomH(uint16_t addr) { return Cartridge::peekRomH(addr); }
    uint8_t peekIO1(uint16_t addr);
    uint8_t readIO1(uint16_t addr);
    uint8_t peekIO2(uint16_t addr);
};


#endif
