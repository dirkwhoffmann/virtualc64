/*!
 * @file        Epyx.h
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

#include "C64.h"

void
EpyxFastLoad::reset()
{
    Cartridge::reset();
    dischargeCapacitor();
}

void
EpyxFastLoad::resetCartConfig()
{
    c64->expansionport.setCartridgeMode(CRT_8K);
}

void
EpyxFastLoad::execute()
{
    checkCapacitor();
}

void
EpyxFastLoad::dischargeCapacitor()
{
    // debug("Discharging capacitor\n");
    
    /* The capacitor will be charged in about 512 cycles (value taken from VICE).
     * We store this value variable 'cycle', so it can be picked up in execute().
     */
    cycle = c64->cpu.cycle + 512;
    
    c64->expansionport.setCartridgeMode(CRT_8K);
}

bool
EpyxFastLoad::checkCapacitor()
{
    
    // debug("Capacitor check: Cartridge continues to live for %ld cycles\n", disable_at_cycle - c64->getCycles());
    
    if (c64->cpu.cycle > cycle) {
        
        // Switch cartridge off
        // Should be really change exrom and game line???
        c64->expansionport.setCartridgeMode(CRT_OFF);
        return false;
    }
    
    return true;
}


uint8_t
EpyxFastLoad::peekRomL(uint16_t addr)
{
    dischargeCapacitor();
    return Cartridge::peekRomL(addr);
}

uint8_t
EpyxFastLoad::peekRomH(uint16_t addr)
{
    dischargeCapacitor();
    return Cartridge::peekRomH(addr);
}

/*
 uint8_t
 EpyxFastLoad::spypeekRomL(uint16_t addr)
 {
 return Cartridge::spypeekRomL(addr);
 }
 
 uint8_t
 EpyxFastLoad::spypeekRomL(uint16_t addr)
 {
 return Cartridge::spypeekRomL(addr);
 }
 */

uint8_t
EpyxFastLoad::peekIO1(uint16_t addr)
{
    dischargeCapacitor();
    return 0;
}

uint8_t
EpyxFastLoad::readIO1(uint16_t addr)
{
    return 0;
}

uint8_t
EpyxFastLoad::peekIO2(uint16_t addr)
{
    // I/O 2 mirrors the last 256 ROM bytes
    // return chip[0][0x1f00 + (addr & 0xff)];
    return packet[0]->peek(0x1f00 + (addr & 0xff));
}
