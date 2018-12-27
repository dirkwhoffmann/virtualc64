/*!
 * @file        Epyx.cpp
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

size_t
EpyxFastLoad::stateSize()
{
    return Cartridge::stateSize() + 8;
}
void
EpyxFastLoad::didLoadFromBuffer(uint8_t **buffer)
{
    Cartridge::didLoadFromBuffer(buffer);
    cycle = read64(buffer);
}

void
EpyxFastLoad::didSaveToBuffer(uint8_t **buffer)
{
    Cartridge::didSaveToBuffer(buffer);
    write64(buffer, cycle);
}

void
EpyxFastLoad::resetCartConfig()
{
    c64->expansionport.setCartridgeMode(CRT_8K);
}

uint8_t
EpyxFastLoad::peekRomL(uint16_t addr)
{
    dischargeCapacitor();
    return Cartridge::peekRomL(addr);
}

uint8_t
EpyxFastLoad::peekIO1(uint16_t addr)
{
    dischargeCapacitor();
    return 0;
}

uint8_t
EpyxFastLoad::peekIO2(uint16_t addr)
{
    // I/O 2 mirrors the last 256 ROM bytes
    return packet[0]->peek(addr & 0x1FFF);
}

void
EpyxFastLoad::execute()
{
    // Switch cartridge off when the capacitor is fully charged
    if (c64->cpu.cycle > cycle) {
        c64->expansionport.setCartridgeMode(CRT_OFF);
    }
}

void
EpyxFastLoad::dischargeCapacitor()
{
    // Switch on cartridge
    c64->expansionport.setCartridgeMode(CRT_8K);
    
    // Schedule cartridge to be switched off in about 512 CPU cycles
    cycle = c64->cpu.cycle + 512;
}
