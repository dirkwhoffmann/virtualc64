// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// This FILE is dual-licensed. You are free to choose between:
//
//     - The GNU General Public License v3 (or any later version)
//     - The Mozilla Public License v2
//
// SPDX-License-Identifier: GPL-3.0-or-later OR MPL-2.0
// -----------------------------------------------------------------------------
/// @file

#pragma once

#include "Reflection.h"
#include "ParCableTypes.h"

namespace vc64 {

//
// Type aliases
//

typedef isize Cylinder;
typedef isize Head;
typedef isize Track;
typedef isize Halftrack;
typedef isize Sector;
typedef isize Block;
typedef isize HeadPos;


//
// Constants
//

#define DRIVE8  0L  // TODO: DON'T USE PREPROCESSOR DIRECTIVE
#define DRIVE9  1L  // TODO: DON'T USE PREPROCESSOR DIRECTIVE


//
// Enumerations
//

/// Drive model
enum_long(DRIVE_TYPE)
{
    DRIVE_VC1541,
    DRIVE_VC1541C,
    DRIVE_VC1541II,
};
typedef DRIVE_TYPE DriveType;

struct DriveTypeEnum : util::Reflection<DriveTypeEnum, DriveType> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = DRIVE_VC1541II;

    static const char *prefix() { return "DRIVE"; }
    static const char *_key(long value)
    {
        switch (value) {

            case DRIVE_VC1541:    return "VC1541";
            case DRIVE_VC1541C:   return "VC1541C";
            case DRIVE_VC1541II:  return "VC1541II";
        }
        return "???";
    }
};

enum_long(DRVRAM)
{
    DRVRAM_NONE,
    DRVRAM_8000_9FFF,
    DRVRAM_6000_7FFF
};
typedef DRVRAM DriveRam;

struct DriveRamEnum : util::Reflection<DriveRamEnum, DriveRam> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = DRVRAM_6000_7FFF;

    static const char *prefix() { return "DRVRAM"; }
    static const char *_key(long value)
    {
        switch (value) {

            case DRVRAM_NONE:       return "DRVRAM_NONE";
            case DRVRAM_8000_9FFF:  return "DRVRAM_8000_9FFF";
            case DRVRAM_6000_7FFF:  return "DRVRAM_6000_7FFF";
        }
        return "???";
    }
};

enum_long(DRVMEM_TYPE)
{
    DRVMEM_NONE,      // Unmapped
    DRVMEM_RAM,
    DRVMEM_EXP,       // Expansion RAM
    DRVMEM_ROM,
    DRVMEM_VIA1,
    DRVMEM_VIA2,
    DRVMEM_PIA        // Peripheral Interface Adapter (Dolphin3)
};
typedef DRVMEM_TYPE DrvMemType;

struct DrvMemTypeEnum : util::Reflection<DrvMemTypeEnum, DrvMemType> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = DRVMEM_PIA;

    static const char *prefix() { return "DRVMEM"; }
    static const char *_key(long value)
    {
        switch (value) {

            case DRVMEM_NONE:       return "DRVMEM_NONE";
            case DRVMEM_RAM:        return "DRVMEM_RAM";
            case DRVMEM_EXP:        return "DRVMEM_EXP";
            case DRVMEM_ROM:        return "DRVMEM_ROM";
            case DRVMEM_VIA1:       return "DRVMEM_VIA1";
            case DRVMEM_VIA2:       return "DRVMEM_VIA2";
            case DRVMEM_PIA:        return "DRVMEM_PIA";
        }
        return "???";
    }
};

enum_long(DISK_INSERTION_STATUS)
{
    DISK_FULLY_EJECTED,
    DISK_PARTIALLY_INSERTED,
    DISK_FULLY_INSERTED,
    DISK_PARTIALLY_EJECTED
};
typedef DISK_INSERTION_STATUS InsertionStatus;

struct InsertionStatusEnum : util::Reflection<InsertionStatusEnum, InsertionStatus> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = DISK_PARTIALLY_EJECTED;

    static const char *prefix() { return "DISK"; }
    static const char *_key(long value)
    {
        switch (value) {

            case DISK_FULLY_EJECTED:           return "FULLY_EJECTED";
            case DISK_PARTIALLY_INSERTED:      return "PARTIALLY_INSERTED";
            case DISK_FULLY_INSERTED:          return "FULLY_INSERTED";
            case DISK_PARTIALLY_EJECTED:       return "PARTIALLY_EJECTED";
        }
        return "???";
    }
};


//
// Structures
//

typedef struct
{
    // General
    bool autoConfig;

    // Hardware
    DriveType type;
    DriveRam ram;
    ParCableType parCable;
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

    // Save Roms
    bool saveRoms;
}
DriveConfig;

typedef struct
{
    isize id;

    bool hasDisk;
    bool hasUnprotectedDisk;
    bool hasProtectedDisk;
    bool hasUnmodifiedDisk;
    bool hasModifiedDisk;

    bool redLED;
    bool greenLED;

    bool spinning;
    bool writing;

    Halftrack halftrack;
    HeadPos offset;
}
DriveInfo;

}
