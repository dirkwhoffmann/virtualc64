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
enum class SIDRevision : long
{
    MOS_6581,
    MOS_8580
};

struct SIDRevisionEnum : Reflection<SIDRevisionEnum, SIDRevision> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = long(SIDRevision::MOS_8580);

    static const char *_key(SIDRevision value)
    {
        switch (value) {

            case SIDRevision::MOS_6581:      return "MOS_6581";
            case SIDRevision::MOS_8580:      return "MOS_8580";
        }
        return "???";
    }
    
    static const char *help(SIDRevision value)
    {
        return "";
    }
};

/// SID backend
enum class SIDEngine : long
{
    RESID
};

struct SIDEngineEnum : Reflection<SIDEngineEnum, SIDEngine> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = long(SIDEngine::RESID);

    static const char *_key(SIDEngine value)
    {
        switch (value) {

            case SIDEngine::RESID:    return "RESID";
        }
        return "???";
    }
    
    static const char *help(SIDEngine value)
    {
        return "";
    }
};

enum class SamplingMethod
{
    // Elements must appear in the same order as in resid::sampling_method
    FAST,
    INTERPOLATE,
    RESAMPLE,
    RESAMPLE_FASTMEM
};

struct SamplingMethodEnum : Reflection<SamplingMethodEnum, SamplingMethod> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = long(SamplingMethod::RESAMPLE_FASTMEM);

    static const char *_key(SamplingMethod value)
    {
        switch (value) {

            case SamplingMethod::FAST:              return "FAST";
            case SamplingMethod::INTERPOLATE:       return "INTERPOLATE";
            case SamplingMethod::RESAMPLE:          return "RESAMPLE";
            case SamplingMethod::RESAMPLE_FASTMEM:  return "RESAMPLE_FASTMEM";
        }
        return "???";
    }
    
    static const char *help(SamplingMethod value)
    {
        return "";
    }
};


//
// Structures
//

/*
typedef struct {

    // Audio sample of the left stereo channel
    float l;

    // Audio sample of the right stereo channel
    float r;
}
SamplePair;
*/

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
