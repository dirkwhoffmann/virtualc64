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

#include "MemoryTypes.h"

struct MemoryTypeEnum : util::Reflection<MemoryTypeEnum, MemoryType> {

    static constexpr long minVal = 1;
    static constexpr long maxVal = M_NONE;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return "M"; }
    static const char *key(MemoryType value)
    {
        switch (value) {

            case M_RAM:     return "RAM";
            case M_CHAR:    return "CHAR";
            case M_KERNAL:  return "KERNAL";
            case M_BASIC:   return "BASIC";
            case M_IO:      return "IO";
            case M_CRTLO:   return "CRTLO";
            case M_CRTHI:   return "CRTHI";
            case M_PP:      return "PP";
            case M_NONE:    return "NONE";
            case M_COUNT:   return "???";
        }
        return "???";
    }
};

struct RamPatternEnum : util::Reflection<RamPatternEnum, RamPattern> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = RAM_PATTERN_RANDOM;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return "RAM_PATTERN"; }
    static const char *key(RamPattern value)
    {
        switch (value) {

            case RAM_PATTERN_VICE:   return "VICE";
            case RAM_PATTERN_CCS:    return "CCS";
            case RAM_PATTERN_ZEROES: return "ZEROES";
            case RAM_PATTERN_ONES:   return "ONES";
            case RAM_PATTERN_RANDOM: return "RANDOM";
        }
        return "???";
    }
};

struct RomTypeEnum : util::Reflection<RomTypeEnum, RomType> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = ROM_TYPE_VC1541;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return "ROM_TYPE"; }
    static const char *key(RomType value)
    {
        switch (value) {

            case ROM_TYPE_BASIC:   return "BASIC";
            case ROM_TYPE_CHAR:    return "CHAR";
            case ROM_TYPE_KERNAL:  return "KERNAL";
            case ROM_TYPE_VC1541:  return "VC1541";
        }
        return "???";
    }
};

struct RomVendorEnum : util::Reflection<RomVendorEnum, RomVendor> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = ROM_VENDOR_OTHER;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return "ROM_VENDOR"; }
    static const char *key(RomVendor value)
    {
        switch (value) {

            case ROM_VENDOR_COMMODORE:  return "COMMODORE";
            case ROM_VENDOR_MEGA65:     return "MEGA65";
            case ROM_VENDOR_OTHER:      return "OTHER";
        }
        return "???";
    }
};
