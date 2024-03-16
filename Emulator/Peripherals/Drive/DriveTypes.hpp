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
#include "Reflection.h"
#include "ParCableTypes.hpp"

#include "DriveTypes.h"

struct DriveTypeEnum : util::Reflection<DriveTypeEnum, DriveType> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = DRIVE_VC1541II;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return "DRIVE"; }
    static const char *key(DriveType value)
    {
        switch (value) {

            case DRIVE_VC1541:    return "VC1541";
            case DRIVE_VC1541C:   return "VC1541C";
            case DRIVE_VC1541II:  return "VC1541II";
        }
        return "???";
    }
};

struct DriveRamEnum : util::Reflection<DriveRamEnum, DriveRam> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = DRVRAM_6000_7FFF;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return "DRVRAM"; }
    static const char *key(DriveType value)
    {
        switch (value) {

            case DRVRAM_NONE:       return "DRVRAM_NONE";
            case DRVRAM_8000_9FFF:  return "DRVRAM_8000_9FFF";
            case DRVRAM_6000_7FFF:  return "DRVRAM_6000_7FFF";
        }
        return "???";
    }
};

struct DrvMemTypeEnum : util::Reflection<DrvMemTypeEnum, DrvMemType> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = DRVMEM_PIA;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return "DRVMEM"; }
    static const char *key(DrvMemType value)
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

struct InsertionStatusEnum : util::Reflection<InsertionStatusEnum, InsertionStatus> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = DISK_PARTIALLY_EJECTED;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return "DISK"; }
    static const char *key(InsertionStatus value)
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
