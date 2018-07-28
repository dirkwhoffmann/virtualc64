//
//  VC1541_types.h
//  V64
//
//  Created by Dirk Hoffmann on 23.07.18.
//

#ifndef VC1541_TYPES_H
#define VC1541_TYPES_H

inline bool isValidDriveNr(unsigned nr) { return nr == 1 || nr == 2; }

/*! @enum     Disk insertion status
 */
typedef enum {
    NOT_INSERTED = 0,
    PARTIALLY_INSERTED,
    FULLY_INSERTED
} DiskInsertionStatus;

#endif
