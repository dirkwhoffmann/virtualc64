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

#include "BasicTypes.h"
#include "utl/storage/RingBuffer.h"

namespace vc64 {

//
// Aliases
//

typedef utl::RingBuffer<short, 2048> SampleStream;


//
// Enumerations
//

/// Chip revision
enum class SIDRevision : long
{
    MOS_6581,
    MOS_8580
};

struct SIDRevisionEnum : utl::Reflectable<SIDRevisionEnum, SIDRevision> {

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

struct SIDEngineEnum : utl::Reflectable<SIDEngineEnum, SIDEngine> {

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

struct SamplingMethodEnum : utl::Reflectable<SamplingMethodEnum, SamplingMethod> {

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
    bool filter;                            ///< Use filter

    // Emlation engine settings
    SIDEngine engine;
    SamplingMethod sampling;                ///< Sampling method

    // Performance
    bool powerSave;

}
SIDConfig;

typedef struct
{
    u8 reg[7];                              ///< Raw voice registers
    u16 frequency;                          ///< Frequency value
    u16 pulseWidth;                         ///< Pulse width value
    u8 waveform;                            ///< Waveform selection bits
    bool ringMod;                           ///< Ring modulation is on
    bool hardSync;                          ///< Hard sync is on
    bool gateBit;                           ///< Gate bit
    bool testBit;                           ///< Test bit
    u8 attackRate;                          ///< Envelope attack rate
    u8 decayRate;                           ///< Envelope decay rate
    u8 sustainRate;                         ///< Envelope sustain level
    u8 releaseRate;                         ///< Envelope release rate
    bool filterEnableBit;                   ///< The voice passes through the filter
}
VoiceInfo;

typedef struct
{
    u8 volume;                              ///< Master volume
    u16 filterCutoff;                       ///< Filter cutoff frequency
    u8 filterResonance;                     ///< Filter resonance
    u8 filterModeBits;                      ///< Filter mode bits
    u8 filterType;                          ///< Filter type
    u8 filterEnableBits;                    ///< Voices routed through the filter
    u8 potX;                                ///< Paddle value X (POTX)
    u8 potY;                                ///< Paddle value Y (POTY)

    VoiceInfo voice[3];                     ///< State of the three voices
}
SIDInfo;

}
