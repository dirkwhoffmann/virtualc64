/*!
 * @header      Memory_types.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   Dirk W. Hoffmann, all rights reserved.
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

#ifndef MEMORY_TYPES_H
#define MEMORY_TYPES_H

//! @brief    Memory source identifiers
/*! @details  The identifiers are used inside the peek and poke lookup tables
 *            to indicate the source and target of a peek or poke operation.
 */
typedef enum {
    M_RAM = 1,
    M_ROM,
    M_CHAR = M_ROM,
    M_KERNAL = M_ROM,
    M_BASIC = M_ROM,
    M_IO,
    M_CRTLO,
    M_CRTHI,
    M_PP,
    M_NONE
} MemoryType;

//! @brief    RAM init pattern type
typedef enum {
    INIT_PATTERN_C64 = 0,
    INIT_PATTERN_C64C = 1
} RamInitPattern;

inline bool isRamInitPattern(RamInitPattern pattern) {
    return (pattern == INIT_PATTERN_C64) || (pattern == INIT_PATTERN_C64C);
}

#endif
