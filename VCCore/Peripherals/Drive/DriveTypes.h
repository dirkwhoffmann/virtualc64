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

#include "ParallelCableTypes.h"

namespace vc64 {

//
// Type aliases
//

/// Cylinder number
typedef isize Cylinder;
/// Head number
typedef isize Head;
/// Track number
typedef isize Track;
/// Halftrack number
typedef isize Halftrack;
/// Sector number
typedef isize Sector;
/// Block number
typedef isize Block;
/// Position of the drive head within a halftrack (in bits)
typedef isize HeadPos;


//
// Constants
//

/// Number of drive 8
static const isize DRIVE8 = 0;
/// Number of drive 9
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

struct DriveTypeEnum : utl::Reflectable<DriveTypeEnum, DriveType> {

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

/// Memory expansion of a drive
enum class DriveRam : long
{
    NONE,
    RANGE_8000_9FFF,                     ///< 8K
    RANGE_6000_7FFF,                     ///< 8K
    RANGE_4000_7FFF                      ///< 16K
};

struct DriveRamEnum : utl::Reflectable<DriveRamEnum, DriveRam> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = long(DriveRam::RANGE_4000_7FFF);

    static const char *_key(DriveRam value)
    {
        switch (value) {

            case DriveRam::NONE:            return "NONE";
            case DriveRam::RANGE_8000_9FFF: return "8000_9FFF";
            case DriveRam::RANGE_6000_7FFF: return "6000_7FFF";
            case DriveRam::RANGE_4000_7FFF: return "4000_7FFF";
        }
        return "???";
    }
    
    static const char *help(DriveRam value)
    {
        return "";
    }
};

/// Memory types of a drive
enum class DrvMemType : long
{
    NONE,                                ///< Unmapped
    RAM,
    EXP,                                 ///< Expansion RAM
    ROM,
    VIA1,
    VIA2,
    PIA                                  ///< Peripheral Interface Adapter (Dolphin3)
};

struct DrvMemTypeEnum : utl::Reflectable<DrvMemTypeEnum, DrvMemType> {

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

/// Insertion state of a disk
enum class InsertionStatus : long
{
    FULLY_EJECTED,
    PARTIALLY_INSERTED,
    FULLY_INSERTED,
    PARTIALLY_EJECTED
};

struct InsertionStatusEnum : utl::Reflectable<InsertionStatusEnum, InsertionStatus> {

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

/// Configuration of a drive
typedef struct
{
    /// General
    bool autoConfig;

    // Hardware
    DriveType type;                      ///< Drive model
    DriveRam ram;                           ///< Drive RAM
    ParCableType parCable;                  ///< Parallel cable
    bool powerSave;                         ///< Take fast paths

    // State
    bool connected;                      ///< The drive is connected to the serial port
    bool switchedOn;                        ///< Power switch

    // Disk handling delays
    isize ejectDelay;                    ///< Disk ejection delay
    isize swapDelay;                        ///< Disk swap delay
    isize insertDelay;                      ///< Disk insertion delay

    // Drive sounds
    i16 pan;                             ///< Stereo pan of the drive sounds
    u8 powerVolume;                         ///< Power switch volume
    u8 stepVolume;                          ///< Head step volume
    u8 insertVolume;                        ///< Disk insertion volume
    u8 ejectVolume;                         ///< Disk ejection volume

    /// Save Roms
    bool saveRoms;
}
DriveConfig;

/// State of a drive
typedef struct
{
    isize id;                               ///< Number of the drive

    bool hasRom;                            ///< A ROM is installed

    bool hasDisk;                           ///< A disk is inserted
    bool hasUnprotectedDisk;                ///< The inserted disk is not write-protected
    bool hasProtectedDisk;                  ///< The inserted disk is write-protected
    bool hasUnmodifiedDisk;                 ///< The inserted disk has no unsaved changes
    bool hasModifiedDisk;                   ///< The inserted disk has unsaved changes

    bool redLED;                            ///< The red LED is on
    bool greenLED;                          ///< The green LED is on

    bool spinning;                          ///< The disk is spinning
    bool writing;                           ///< The drive is writing

    Halftrack halftrack;                    ///< Current head position (halftrack)
    HeadPos offset;                         ///< Head offset within the track
}
DriveInfo;

}
