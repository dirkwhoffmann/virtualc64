// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
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
    MOS_8580,
    SIDREV_COUNT
};
typedef SIDREV SIDRevision;

#ifdef __cplusplus
struct SIDRevisionEnum : util::Reflection<SIDRevisionEnum, SIDRevision> {
    
    static bool isValid(long value)
    {
        return (unsigned long)value < SIDREV_COUNT;
    }
    
    static const char *prefix() { return nullptr; }
    static const char *key(SIDRevision value)
    {
        switch (value) {
                
            case MOS_6581:      return "MOS_6581";
            case MOS_8580:      return "MOS_8580";
            case SIDREV_COUNT:  return "???";
        }
        return "???";
    }
};
#endif

enum_long(SIDENGINE)
{
    SIDENGINE_FASTSID,
    SIDENGINE_RESID,
    SIDENGINE_COUNT
};
typedef SIDENGINE SIDEngine;

#ifdef __cplusplus
struct SIDEngineEnum : util::Reflection<SIDEngineEnum, SIDEngine> {
    
    static bool isValid(long value)
    {
        return (unsigned long)value < SIDENGINE_COUNT;
    }
    
    static const char *prefix() { return "SIDENGINE"; }
    static const char *key(SIDEngine value)
    {
        switch (value) {
                
            case SIDENGINE_FASTSID:  return "FASTSID";
            case SIDENGINE_RESID:    return "RESID";
            case SIDENGINE_COUNT:    return "???";
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
    SAMPLING_RESAMPLE_FASTMEM,
    SAMPLING_COUNT
};
typedef SAMPLING SamplingMethod;

#ifdef __cplusplus
struct SamplingMethodEnum : util::Reflection<SamplingMethodEnum, SamplingMethod> {
    
    static bool isValid(long value)
    {
        return (unsigned long)value < SAMPLING_COUNT;
    }
    
    static const char *prefix() { return "SAMPLING"; }
    static const char *key(SamplingMethod value)
    {
        switch (value) {
                
            case SAMPLING_FAST:              return "FAST";
            case SAMPLING_INTERPOLATE:       return "INTERPOLATE";
            case SAMPLING_RESAMPLE:          return "RESAMPLE";
            case SAMPLING_RESAMPLE_FASTMEM:  return "RESAMPLE FASTMEM";
            case SAMPLING_COUNT:             return "???";
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
SIDConfig;

typedef struct
{
    // Current ring buffer fill level
    double fillLevel;
    
    // Number of buffer underflows since power up
    u64 bufferUnderflows;

    // Number of buffer overflows since power up
    u64 bufferOverflows;
}
SIDStats;

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
