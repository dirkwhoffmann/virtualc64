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

#pragma once

#include "Aliases.h"
#include "Reflection.h"

//
// Enumerations
//

enum_long(SIDREV)
{
    MOS_6581,
    MOS_8580
};
typedef SIDREV SIDRevision;

#ifdef __cplusplus
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
#endif

enum_long(SIDENGINE)
{
    SIDENGINE_FASTSID,
    SIDENGINE_RESID
};
typedef SIDENGINE SIDEngine;

#ifdef __cplusplus
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
#endif

enum_long(SAMPLING)
{
    // Elements must appear in the same order as in resid::sampling_method
    SAMPLING_FAST,
    SAMPLING_INTERPOLATE,
    SAMPLING_RESAMPLE,
    SAMPLING_RESAMPLE_FASTMEM
};
typedef SAMPLING SamplingMethod;

#ifdef __cplusplus
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
#endif


//
// Structures
//

typedef struct
{
    // Hardware settings
    SIDRevision revision;
    bool powerSave;
    u8 enabled;
    u16 address[4];
    bool filter;
    
    // Emlation engine settings
    SIDEngine engine;
    SamplingMethod sampling;
    
    // Master volume (left and right channel)
    i64 volL;
    i64 volR;

    // Channel volumes and pan factors
    i64 vol[4];
    i64 pan[4];
}
MuxerConfig;

typedef struct
{
    // Current ring buffer fill level
    double fillLevel;
    
    // Number of buffer underflows since power up
    u64 bufferUnderflows;

    // Number of buffer overflows since power up
    u64 bufferOverflows;
}
MuxerStats;

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
}
SIDInfo;
