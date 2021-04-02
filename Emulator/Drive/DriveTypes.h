// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "Aliases.h"
#include "Reflection.h"

//
// Enumerations
//

enum_long(DRIVE_ID)
{
    DRIVE8 = 8,
    DRIVE9 = 9
};
typedef DRIVE_ID DriveID;

enum_long(DRIVE_MODEL)
{
    DRIVE_MODEL_VC1541II,
    DRIVE_MODEL_COUNT
};
typedef DRIVE_MODEL DriveModel;

#ifdef __cplusplus
struct DriveTypeEnum : util::Reflection<DriveTypeEnum, DriveModel> {
    
    static bool isValid(long value)
    {
        return (unsigned long)value < DRIVE_MODEL_COUNT;
    }
    
    static const char *prefix() { return "DRIVE_MODEL"; }
    static const char *key(DriveModel value)
    {
        switch (value) {
                
            case DRIVE_MODEL_VC1541II:  return "VC1541II";
            case DRIVE_MODEL_COUNT:     return "???";
        }
        return "???";
    }
};
#endif

enum_long(DISK_INSERTION_STATUS)
{
    DISK_FULLY_EJECTED,
    DISK_PARTIALLY_INSERTED,
    DISK_FULLY_INSERTED,
    DISK_PARTIALLY_EJECTED,
    DISK_INSERTION_STATUS_COUNT
};
typedef DISK_INSERTION_STATUS InsertionStatus;

#ifdef __cplusplus
struct InsertionStatusEnum : util::Reflection<InsertionStatusEnum, InsertionStatus> {
    
    static bool isValid(long value)
    {
        return (unsigned)value < DISK_INSERTION_STATUS_COUNT;
    }
    
    static const char *prefix() { return "DISK"; }
    static const char *key(InsertionStatus value)
    {
        switch (value) {
                
            case DISK_FULLY_EJECTED:           return "FULLY_EJECTED";
            case DISK_PARTIALLY_INSERTED:      return "PARTIALLY_INSERTED";
            case DISK_FULLY_INSERTED:          return "FULLY_INSERTED";
            case DISK_PARTIALLY_EJECTED:       return "PARTIALLY_EJECTED";
            case DISK_INSERTION_STATUS_COUNT:  return "???";
        }
        return "???";
    }
};
#endif


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
