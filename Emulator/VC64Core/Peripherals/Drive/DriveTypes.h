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
#include "ParallelCableTypes.h"

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

static const isize DRIVE8 = 0;
static const isize DRIVE9 = 1;


//
// Enumerations
//

/// Drive model
enum class DriveType : long
{
    VC1541,
    VC1541C,
    VC1541II,
};

struct DriveTypeEnum : util::Reflection<DriveTypeEnum, DriveType> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = long(DriveType::VC1541II);

    static const char *_key(DriveType value)
    {
        switch (value) {

            case DriveType::VC1541:    return "VC1541";
            case DriveType::VC1541C:   return "VC1541C";
            case DriveType::VC1541II:  return "VC1541II";
        }
        return "???";
    }
    
    static const char *help(DriveType value)
    {
        return "";
    }
};

enum class DriveRam : long
{
    NONE,
    RANGE_8000_9FFF,
    RANGE_6000_7FFF
};

struct DriveRamEnum : util::Reflection<DriveRamEnum, DriveRam> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = long(DriveRam::RANGE_6000_7FFF);

    static const char *_key(DriveRam value)
    {
        switch (value) {

            case DriveRam::NONE:            return "NONE";
            case DriveRam::RANGE_8000_9FFF: return "8000_9FFF";
            case DriveRam::RANGE_6000_7FFF: return "6000_7FFF";
        }
        return "???";
    }
    
    static const char *help(DriveRam value)
    {
        return "";
    }
};

enum class DrvMemType : long
{
    NONE,      // Unmapped
    RAM,
    EXP,       // Expansion RAM
    ROM,
    VIA1,
    VIA2,
    PIA        // Peripheral Interface Adapter (Dolphin3)
};

struct DrvMemTypeEnum : util::Reflection<DrvMemTypeEnum, DrvMemType> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = long(DrvMemType::PIA);

    static const char *_key(DrvMemType value)
    {
        switch (value) {

            case DrvMemType::NONE:       return "NONE";
            case DrvMemType::RAM:        return "RAM";
            case DrvMemType::EXP:        return "EXP";
            case DrvMemType::ROM:        return "ROM";
            case DrvMemType::VIA1:       return "VIA1";
            case DrvMemType::VIA2:       return "VIA2";
            case DrvMemType::PIA:        return "PIA";
        }
        return "???";
    }
    
    static const char *help(DrvMemType value)
    {
        return "";
    }
};

enum class InsertionStatus : long
{
    FULLY_EJECTED,
    PARTIALLY_INSERTED,
    FULLY_INSERTED,
    PARTIALLY_EJECTED
};

struct InsertionStatusEnum : util::Reflection<InsertionStatusEnum, InsertionStatus> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = long(InsertionStatus::PARTIALLY_EJECTED);

    static const char *_key(InsertionStatus value)
    {
        switch (value) {

            case InsertionStatus::FULLY_EJECTED:        return "FULLY_EJECTED";
            case InsertionStatus::PARTIALLY_INSERTED:   return "PARTIALLY_INSERTED";
            case InsertionStatus::FULLY_INSERTED:       return "FULLY_INSERTED";
            case InsertionStatus::PARTIALLY_EJECTED:    return "PARTIALLY_EJECTED";
        }
        return "???";
    }
    
    static const char *help(InsertionStatus value)
    {
        return "";
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
