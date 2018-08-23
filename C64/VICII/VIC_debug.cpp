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
