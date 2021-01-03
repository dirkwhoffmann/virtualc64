// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef DRIVE_PRIVATE_H
#define DRIVE_PRIVATE_H

//
// Reflection APIs
//

struct DriveTypeEnum : Reflection<DriveTypeEnum, DriveModel> {
    
    static bool isValid(long value)
    {
        return (unsigned long)value < DRIVE_MODEL_COUNT;
    }
    
    static const char *prefix() { return "DRIVE"; }
    static const char *key(DriveModel value)
    {
        switch (value) {
                
            case DRIVE_MODEL_VC1541II:  return "VC1541II";
            case DRIVE_MODEL_COUNT:     return "???";
        }
        return "???";
    }
};

struct InsertionStatusEnum : Reflection<InsertionStatusEnum, InsertionStatus> {
    
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
