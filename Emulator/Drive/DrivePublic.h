// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef VC1541_PUBLIC_H
#define VC1541_PUBLIC_H

//
// Enumerations
//

enum_long(DRIVE_ID)
{
    DRIVE8 = 8,
    DRIVE9 = 9
};
typedef DRIVE_ID DriveID;

inline bool isDriveID(long value)
{
    return (unsigned long)value <= DRIVE9;
}

enum_long(DRIVE_MODEL)
{
    DRIVE_MODEL_VC1541II,
    DRIVE_MODEL_COUNT
};
typedef DRIVE_MODEL DriveModel;

enum_long(DISK_INSERTION_STATUS)
{
    DISK_FULLY_EJECTED,
    DISK_PARTIALLY_INSERTED,
    DISK_FULLY_INSERTED,
    DISK_PARTIALLY_EJECTED,
    DISK_INSERTION_STATUS_COUNT
};
typedef DISK_INSERTION_STATUS InsertionStatus;

//
// Structures
//

typedef struct
{
    DriveModel type;
    bool connected;
    bool switchedOn;
}
DriveConfig;

#endif
