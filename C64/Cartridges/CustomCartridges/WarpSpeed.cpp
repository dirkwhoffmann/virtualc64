/*!
 * @file        WarpSpeed.cpp
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
WarpSpeed::resetCartConfig()
{
    c64->expansionport.setCartridgeMode(CRT_16K);
}

uint8_t
WarpSpeed::peekIO1(uint16_t addr)
{
    return Cartridge::peekRomL(0x1E00 | (addr & 0xFF));
}

uint8_t
WarpSpeed::peekIO2(uint16_t addr)
{
    return Cartridge::peekRomL(0x1F00 | (addr & 0xFF));
}

void
WarpSpeed::pokeIO1(uint16_t addr, uint8_t value)
{
    c64->expansionport.setCartridgeMode(CRT_16K);
}

void
WarpSpeed::pokeIO2(uint16_t addr, uint8_t value)
{
    c64->expansionport.setCartridgeMode(CRT_OFF);
}

