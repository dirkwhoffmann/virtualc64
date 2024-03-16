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

#include "Aliases.h"
#include "ParCableTypes.h"

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

enum_long(DRVRAM)
{
    DRVRAM_NONE,
    DRVRAM_8000_9FFF,
    DRVRAM_6000_7FFF
};
typedef DRVRAM DriveRam;

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

enum_long(DISK_INSERTION_STATUS)
{
    DISK_FULLY_EJECTED,
    DISK_PARTIALLY_INSERTED,
    DISK_FULLY_INSERTED,
    DISK_PARTIALLY_EJECTED
};
typedef DISK_INSERTION_STATUS InsertionStatus;

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
