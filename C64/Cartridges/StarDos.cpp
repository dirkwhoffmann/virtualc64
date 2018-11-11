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
    if (voltage > 2700000 /* 2.7V */) {
        // romIsVisible = true;
        enableROML();
    }
    debug("charge %lld (ROML %s)\n", voltage, voltage > 2700000 ? "ON" : "OFF");
}

void
StarDos::discharge()
{
    updateVoltage();
    voltage -= MIN(voltage, 78125);
    if (voltage < 1400000 /* 1.4V */) {
        // romIsVisible = false;
        disableROML();
    }
    
    debug("discharge %lld (ROML %s)\n", voltage, voltage < 1400000 ? "OFF" : "ON");

}

void
StarDos::enableROML()
{
    c64->expansionport.setExromLine(0);
}

void
StarDos::disableROML()
{
    c64->expansionport.setExromLine(1);
}

void
StarDos::updatePeekPokeLookupTables()
{
    // Replace Kernel by the StarDos kernel
    if (c64->mem.peekSrc[0xE] == M_KERNAL) {
        c64->mem.peekSrc[0xE] = M_CRTHI;
        c64->mem.peekSrc[0xF] = M_CRTHI;
    }
}

/*
uint8_t
StarDos::peekRomL(uint16_t addr)
{
    if (romIsVisible) {
        return Cartridge::peekRomL(addr);
    } else {
        return c64->mem.ram[addr];
    }
}
*/

