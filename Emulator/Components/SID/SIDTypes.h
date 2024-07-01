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
#include "RingBuffer.h"

namespace vc64 {

//
// Aliases
//

typedef util::RingBuffer<short, 2048> SampleStream;


//
// Enumerations
//

/// Chip revision
enum_long(SIDREV)
{
    MOS_6581,
    MOS_8580
};
typedef SIDREV SIDRevision;

struct SIDRevisionEnum : util::Reflection<SIDRevisionEnum, SIDRevision> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = MOS_8580;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return nullptr; }
    static const char *key(long value)
    {
        switch (value) {

            case MOS_6581:      return "MOS_6581";
            case MOS_8580:      return "MOS_8580";
        }
        return "???";
    }
};

/// SID backend
enum_long(SIDENGINE)
{
    SIDENGINE_RESID
};
typedef SIDENGINE SIDEngine;

struct SIDEngineEnum : util::Reflection<SIDEngineEnum, SIDEngine> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = SIDENGINE_RESID;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return "SIDENGINE"; }
    static const char *key(long value)
    {
        switch (value) {

            case SIDENGINE_RESID:    return "RESID";
        }
        return "???";
    }
};

enum_long(SAMPLING)
{
    // Elements must appear in the same order as in resid::sampling_method
    SAMPLING_FAST,
    SAMPLING_INTERPOLATE,
    SAMPLING_RESAMPLE,
    SAMPLING_RESAMPLE_FASTMEM
};
typedef SAMPLING SamplingMethod;

struct SamplingMethodEnum : util::Reflection<SamplingMethodEnum, SamplingMethod> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = SAMPLING_RESAMPLE_FASTMEM;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return "SAMPLING"; }
    static const char *key(long value)
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


//
// Structures
//

typedef struct {

    // Audio sample of the left stereo channel
    float l;

    // Audio sample of the right stereo channel
    float r;
}
SamplePair;

typedef struct
{
    // Indicates if this SID is plugged in
    bool enabled;

    // Location of the SID registers in memory
    u16 address;

    // Hardware settings
    SIDRevision revision;
    bool filter;

    // Emlation engine settings
    SIDEngine engine;
    SamplingMethod sampling;

    // Performance
    bool powerSave;

}
SIDConfig;

typedef struct
{
    u8 reg[7];
    u16 frequency;
    u16 pulseWidth;
    u8 waveform;
    bool ringMod;
    bool hardSync;
    bool gateBit;
    bool testBit;
    u8 attackRate;
    u8 decayRate;
    u8 sustainRate;
    u8 releaseRate;
    bool filterEnableBit;
}
VoiceInfo;

typedef struct
{
    u8 volume;
    u16 filterCutoff;
    u8 filterResonance;
    u8 filterModeBits;
    u8 filterType;
    u8 filterEnableBits;
    u8 potX;
    u8 potY;

    VoiceInfo voice[3];
}
SIDInfo;

}
