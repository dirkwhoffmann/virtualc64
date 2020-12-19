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

enum_long(DriveID)
{
    DRIVE8 = 8,
    DRIVE9 = 9
};

inline bool isDriveID(long value)
{
    return value >= DRIVE8 && value <= DRIVE9;
}

enum_long(DriveType)
{
    DRIVE_VC1541II
};

inline bool isDriveType(long value)
{
    return value == DRIVE_VC1541II;
}

enum_long(InsertionStatus)
{
    FULLY_EJECTED,
    PARTIALLY_INSERTED,
    FULLY_INSERTED,
    PARTIALLY_EJECTED
};

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
