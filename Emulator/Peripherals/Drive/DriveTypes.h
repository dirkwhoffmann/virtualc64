// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
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

enum_long(DRIVE_TYPE)
{
    DRIVE_VC1541II,
    DRIVE_COUNT
};
typedef DRIVE_TYPE DriveType;

#ifdef __cplusplus
struct DriveTypeEnum : util::Reflection<DriveTypeEnum, DriveType> {
    
    static bool isValid(long value)
    {
        return (unsigned long)value < DRIVE_COUNT;
    }
    
    static const char *prefix() { return "DRIVE"; }
    static const char *key(DriveType value)
    {
        switch (value) {
                
            case DRIVE_VC1541II:  return "VC1541II";
            case DRIVE_COUNT:     return "???";
        }
        return "???";
    }
};
#endif

enum_long(DRVMEM_TYPE)
{
    DRVMEM_NONE,      // Unmapped
    DRVMEM_RAM,
    DRVMEM_EXP,       // Expansion RAM
    DRVMEM_ROM,
    DRVMEM_VIA1,
    DRVMEM_VIA2,
    DRVMEM_PIA        // Peripheral Interface Adapter (not supported, yet)
};
typedef DRVMEM_TYPE DrvMemType;

#ifdef __cplusplus
struct DrvMemTypeEnum : util::Reflection<DrvMemTypeEnum, DrvMemType> {
    
    static bool isValid(long value)
    {
        return (unsigned long)value <= DRVMEM_PIA;
    }
    
    static const char *prefix() { return "DRVMEM"; }
    static const char *key(DrvMemType value)
    {
        switch (value) {
                
            case DRVMEM_NONE:       return "DRVMEM_NONE";
            case DRVMEM_RAM:        return "DRVMEM_RAM";
            case DRVMEM_ROM:        return "DRVMEM_ROM";
            case DRVMEM_VIA1:       return "DRVMEM_VIA1";
            case DRVMEM_VIA2:       return "DRVMEM_VIA2";
            case DRVMEM_PIA:        return "DRVMEM_PIA";
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
    DriveType type;
    bool powerSave;

    // State
    bool connected;
    bool switchedOn;
        
    // Disk handling delays
    isize ejectDelay;
    isize swapDelay;
    isize insertDelay;
    
    // Drive sounds
    i16 pan;
    u8 powerVolume;
    u8 stepVolume;
    u8 insertVolume;
    u8 ejectVolume;
}
DriveConfig;
