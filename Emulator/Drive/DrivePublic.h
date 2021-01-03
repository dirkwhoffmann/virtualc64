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

enum_long(DRIVE_IDENTIFIER)
{
    DRIVE8 = 8,
    DRIVE9 = 9
};
typedef DRIVE_IDENTIFIER DriveID;

inline bool isDriveID(long value)
{
    return (unsigned long)value <= DRIVE9;
}

enum_long(DRIVE_TYPE)
{
    DRIVE_VC1541II
};
typedef DRIVE_TYPE DriveType;

inline bool isDriveType(long value)
{
    return (unsigned long)value <= DRIVE_VC1541II;
}

inline const char *DriveTypeName(DriveType value)
{
    switch (value) {
            
        case DRIVE_VC1541II:     return "VC1541II";
    }
    return "???";
}

enum_long(DISK_INSERTION_STATUS)
{
    DISK_FULLY_EJECTED,
    DISK_PARTIALLY_INSERTED,
    DISK_FULLY_INSERTED,
    DISK_PARTIALLY_EJECTED
};
typedef DISK_INSERTION_STATUS InsertionStatus;

inline bool isInsertionStatus(long value)
{
    return (unsigned)value <= DISK_PARTIALLY_EJECTED;
}

inline const char *InsertionStatusName(InsertionStatus value)
{
    switch (value) {
            
        case DISK_FULLY_EJECTED:       return "FULLY_EJECTED";
        case DISK_PARTIALLY_INSERTED:  return "PARTIALLY_INSERTED";
        case DISK_FULLY_INSERTED:      return "FULLY_INSERTED";
        case DISK_PARTIALLY_EJECTED:   return "PARTIALLY_EJECTED";
    }
    return "???";
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
