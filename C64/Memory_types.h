//
//  Memory_types.h
//  V64
//
//  Created by Dirk Hoffmann on 15.04.18.
//

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
} MemorySource;

#endif
