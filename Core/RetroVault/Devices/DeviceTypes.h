// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "utl/abilities/Reflectable.h"

namespace retro::vault {

using namespace utl;

//
// Aliases
//

// Track devices
using BlockNr  = isize;
using CylNr    = isize;
using HeadNr   = isize;
using SectorNr = isize;
using TrackNr  = isize;


//
// Structures
//

struct TSLink
{
    TrackNr t;
    SectorNr s;
};

constexpr bool operator==(const TSLink &a, const TSLink &b) noexcept {
    return a.t == b.t && a.s == b.s;
}
constexpr bool operator!=(const TSLink &a, const TSLink &b) noexcept {
    return !(a == b);
}


//
// Constants
//

// Hard drive geometry limits
static const isize HDR_C_MIN =  16;
static const isize HDR_C_MAX =  16384;
static const isize HDR_H_MIN =  1;
static const isize HDR_H_MAX =  16;
static const isize HDR_S_MIN =  16;
static const isize HDR_S_MAX =  63;


//
// Enumerations
//

enum class Diameter : long
{
    INCH_35,
    INCH_525,
    INCH_8
};

struct DiameterEnum : Reflectable<DiameterEnum, Diameter>
{
    static constexpr long minVal = 0;
    static constexpr long maxVal = long(Diameter::INCH_8);

    static const char *_key(Diameter value)
    {
        switch (value) {

            case Diameter::INCH_35:     return "INCH_35";
            case Diameter::INCH_525:    return "INCH_525";
            case Diameter::INCH_8:      return "INCH_8";
        }
        return "???";
    }
    static const char *help(Diameter value)
    {
        switch (value) {

            case Diameter::INCH_35:     return "3.5\" floppy disk";
            case Diameter::INCH_525:    return "5.25\" floppy disk";
            case Diameter::INCH_8:      return "8\" floppy disk";
        }
        return "???";
    }
};

enum class Density
{
    SD,
    DD,
    HD
};

struct DensityEnum : Reflectable<DensityEnum, Density>
{
    static constexpr long minVal = 0;
    static constexpr long maxVal = long(Density::HD);

    static const char *_key(Density value)
    {
        switch (value) {

            case Density::SD:     return "SD";
            case Density::DD:     return "DD";
            case Density::HD:     return "HD";
        }
        return "???";
    }
    static const char *help(Density value)
    {
        switch (value) {

            case Density::SD:     return "Single Density";
            case Density::DD:     return "Double Density";
            case Density::HD:     return "High Density";
        }
        return "???";
    }
};

}
