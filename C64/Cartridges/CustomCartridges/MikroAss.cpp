/*!
 * @file        MikroAss.cpp
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

uint8_t
MikroAss::peekIO1(uint16_t addr)
{
    // debug("MikroAss::peekIO1(%x)", addr);
    return peekRomL(0x1E00 | LO_BYTE(addr));
}

uint8_t
MikroAss::peekIO2(uint16_t addr)
{
    // debug("MikroAss::peekIO2(%x)", addr);
    return peekRomL(0x1F00 | LO_BYTE(addr));
}
