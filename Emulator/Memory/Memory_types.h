// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

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
