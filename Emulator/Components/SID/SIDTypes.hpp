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
#include "Reflection.hpp"

#include "SIDTypes.h"

struct SIDRevisionEnum : util::Reflection<SIDRevisionEnum, SIDRevision> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = MOS_8580;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return nullptr; }
    static const char *key(SIDRevision value)
    {
        switch (value) {

            case MOS_6581:      return "MOS_6581";
            case MOS_8580:      return "MOS_8580";
        }
        return "???";
    }
};

struct SIDEngineEnum : util::Reflection<SIDEngineEnum, SIDEngine> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = SIDENGINE_RESID;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return "SIDENGINE"; }
    static const char *key(SIDEngine value)
    {
        switch (value) {

            case SIDENGINE_FASTSID:  return "FASTSID";
            case SIDENGINE_RESID:    return "RESID";
        }
        return "???";
    }
};

struct SamplingMethodEnum : util::Reflection<SamplingMethodEnum, SamplingMethod> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = SAMPLING_RESAMPLE_FASTMEM;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return "SAMPLING"; }
    static const char *key(SamplingMethod value)
    {
        switch (value) {

            case SAMPLING_FAST:              return "FAST";
            case SAMPLING_INTERPOLATE:       return "INTERPOLATE";
            case SAMPLING_RESAMPLE:          return "RESAMPLE";
            case SAMPLING_RESAMPLE_FASTMEM:  return "RESAMPLE_FASTMEM";
        }
        return "???";
    }
};
