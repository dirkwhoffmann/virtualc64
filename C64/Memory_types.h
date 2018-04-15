//
//  Memory_types.h
//  V64
//
//  Created by Dirk Hoffmann on 15.04.18.
//

#ifndef MEMORY_TYPES_H
#define MEMORY_TYPES_H

/*! @brief    Memory type
 *  @details  This datatype defines a constant value for the different kinds of memory.
 */
typedef enum {
    MEM_RAM,
    MEM_ROM,
    MEM_IO
} MemoryType;

#endif
