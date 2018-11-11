/*!
 * @file        StarDos.h
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
StarDos::reset()
{
    Cartridge::reset();
    voltage = 0;
    latestVoltageUpdate = 0;
}

void
StarDos::resetCartConfig()
{
    debug("Enabling Ultimax mode.\n");
    
    // VIC will always see RAM
    c64->expansionport.setGameLinePhi1(1);
    c64->expansionport.setExromLinePhi1(1);

    // CPU acts in Ultimax mode
    c64->expansionport.setGameLinePhi2(0);
    c64->expansionport.setExromLinePhi2(1);
    
}

void
StarDos::updateVoltage()
{
    // If the capacitor is untouched, it slowly raises to 2.0V
    
    if (voltage < 2000000 /* 2.0V */) {
        uint64_t elapsedCycles = c64->cpu.cycle - latestVoltageUpdate;
        voltage += MIN(2000000 - voltage, elapsedCycles * 2);
    }
    latestVoltageUpdate = c64->cpu.cycle;
}

void
StarDos::charge()
{
    updateVoltage();
    voltage += MIN(5000000 /* 5.0V */ - voltage, 78125);
    if (voltage > 2700000 /* 2.7V */) enableROML = true;
}

void
StarDos::discharge()
{
    updateVoltage();
    voltage -= MIN(voltage, 78125);
    if (voltage < 1400000 /* 1.4V */) enableROML = false;
}

uint8_t
StarDos::peekRomL(uint16_t addr)
{
    if (enableROML) {
        return Cartridge::peekRomL(addr);
    } else {
        return c64->mem.ram[addr];
    }
}
