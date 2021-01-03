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

struct DriveTypeEnum : Reflection<DriveTypeEnum, DriveType> {
    
    static bool isValid(long value)
    {
        return (unsigned long)value <= DRIVE_VC1541II;
    }
    
    static const char *prefix() { return "DRIVE"; }
    static const char *key(DriveType value)
    {
        switch (value) {
                
            case DRIVE_VC1541II:  return "VC1541II";
        }
        return "???";
    }
};

struct InsertionStatusEnum : Reflection<InsertionStatusEnum, InsertionStatus> {
    
    static bool isValid(long value)
    {
        return (unsigned)value <= DISK_PARTIALLY_EJECTED;
    }
    
    static const char *prefix() { return "DISK"; }
    static const char *key(InsertionStatus value)
    {
        switch (value) {
                
            case DISK_FULLY_EJECTED:       return "FULLY_EJECTED";
            case DISK_PARTIALLY_INSERTED:  return "PARTIALLY_INSERTED";
            case DISK_FULLY_INSERTED:      return "FULLY_INSERTED";
            case DISK_PARTIALLY_EJECTED:   return "PARTIALLY_EJECTED";
        }
        return "???";
    }
};

#endif
