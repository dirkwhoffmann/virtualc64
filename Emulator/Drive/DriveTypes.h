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
    FULLY_EJECTED,
    PARTIALLY_INSERTED,
    FULLY_INSERTED,
    PARTIALLY_EJECTED
}
InsertionStatus;

inline bool isInsertionStatus(long value)
{
    return value >= FULLY_EJECTED && value <= PARTIALLY_EJECTED;
}


//
// Structures
//

typedef struct
{
    DriveType type;
    bool connected;
    bool switchedOn;
}
DriveConfig;

#endif
