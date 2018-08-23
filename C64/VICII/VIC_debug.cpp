/*!
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   Dirk W. Hoffmann, 2018
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

#include "C64.h"

/* All functions in this file are meant to be called by the debugger, only.
 * As the debugger is running in another thread, all code that modifies the
 * the VICII internals is wrapped inside a suspend()/resume() block.
 */

void
VIC::setMemoryBankAddr(uint16_t addr)
{
    assert(addr % 0x4000 == 0);
    
    c64->suspend();
    bankAddr = addr;
    c64->resume();
}

void
VIC::setDisplayMode(DisplayMode m) {
    
    c64->suspend();
    control1.write((control1.current() & ~0x60) | (m & 0x60));
    control2.write((control2.current() & ~0x10) | (m & 0x10));
    c64->resume();
}




//
// Sprites
//

void
VIC::setSpriteX(unsigned nr, uint16_t x)
{
    assert(nr < 8);
    
    x = MIN(x, 511);
    
    c64->suspend();
    p.spriteX[nr] = x;
    iomem[2*nr] = x & 0xFF;
    if (x & 0x100) SET_BIT(iomem[0x10],nr); else CLR_BIT(iomem[0x10],nr);
    c64->resume();
}

void
VIC::setSpriteY(unsigned nr, uint8_t y)
{
    assert(nr < 8);
    
    c64->suspend();
    iomem[1+2*nr] = y;
    c64->resume();
}

void
VIC::setSpriteColor(unsigned nr, uint8_t color)
{
    assert(nr < 8);
    
    c64->suspend();
    sprColor[nr].write(color);
    c64->resume();
}



