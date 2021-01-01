// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef SID_TYPES_H
#define SID_TYPES_H

//
// Enumerations
//

enum_long(SIDRev)
{
    SIDRev_MOS6581,
    SIDRev_MOS8580
};

inline bool isSIDRev(long value)
{
    return (unsigned long)value <= SIDRev_MOS8580;
}

inline const char *SIDRevName(SIDRev type)
{
    assert(isSIDRev(type));
    
    switch (type) {
            
        case SIDRev_MOS6581:  return "MOS_6581";
        case SIDRev_MOS8580:  return "MOS_8580";
        default:              return "???";
    }
}

enum_long(SIDEngine)
{
    SIDEngine_FASTSID,
    SIDEngine_RESID
};

inline bool isSIDEngine(long value)
{
    return (unsigned long)value <= SIDEngine_RESID;
}

inline const char *SIDEngineName(SIDEngine value)
{
    assert(isSIDEngine(value));
    
    switch (value) {
            
        case SIDEngine_FASTSID:  return "FASTSID";
        case SIDEngine_RESID:    return "RESID";
        default:                 return "???";
    }
}

// This enum reflects enum "sampling_method" used by reSID.
enum_long(SamplingMethod)
{
    SamplingMethod_FAST,
    SamplingMethod_INTERPOLATE,
    SamplingMethod_RESAMPLE,
    SamplingMethod_RESAMPLE_FASTMEM
};

inline bool isSamplingMethod(long value)
{
    return (unsigned long)value <= SamplingMethod_RESAMPLE_FASTMEM;
}

inline const char *SamplingMethodName(SamplingMethod method)
{
    assert(isSamplingMethod(method));
    
    switch (method) {
            
        case SamplingMethod_FAST:              return "FAST";
        case SamplingMethod_INTERPOLATE:       return "INTERPOLATE";
        case SamplingMethod_RESAMPLE:          return "RESAMPLE";
        case SamplingMethod_RESAMPLE_FASTMEM:  return "RESAMPLE FASTMEM";
        default:                               return "???";
    }
}

//
// Structures
//

typedef struct
{
    // Hardware settings
    SIDRev revision;
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

#endif
