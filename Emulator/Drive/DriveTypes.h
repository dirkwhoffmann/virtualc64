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

//
// Enumerations
//

typedef enum : long
{
    DRIVE8 = 8,
    DRIVE9 = 9
}
DriveID;

inline bool isDriveID(long value)
{
    return value >= DRIVE8 && value <= DRIVE9;
}

typedef enum : long
{
    DRIVE_VC1541II
}
DriveType;

inline bool isDriveType(long value)
{
    return value == DRIVE_VC1541II;
}

typedef enum : long
{
    NOT_INSERTED,
    PARTIALLY_INSERTED,
    FULLY_INSERTED
}
InsertionStatus;

inline bool isInsertionStatus(long value)
{
    return value >= NOT_INSERTED && value <= FULLY_INSERTED;
}

//
// Structures
//

typedef struct
{
    DriveType type;
    bool connected;
}
DriveConfig;

#endif
