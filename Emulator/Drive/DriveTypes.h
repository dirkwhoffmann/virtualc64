// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

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
