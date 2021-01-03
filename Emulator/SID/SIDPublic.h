// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef SID_PUBLIC_H
#define SID_PUBLIC_H

//
// Enumerations
//

enum_long(SIDREV)
{
    MOS_6581,
    MOS_8580
};
typedef SIDREV SIDRevision;

inline bool isSIDRevision(long value)
{
    return (unsigned long)value <= MOS_8580;
}

inline const char *SIDRevisionName(SIDRevision type)
{
    switch (type) {
            
        case MOS_6581:  return "6581";
        case MOS_8580:  return "8580";
    }
    return "???";
}

enum_long(SIDENGINE)
{
    SIDENGINE_FASTSID,
    SIDENGINE_RESID
};
typedef SIDENGINE SIDEngine;

inline bool isSIDEngine(long value)
{
    return (unsigned long)value <= SIDENGINE_RESID;
}

inline const char *SIDEngineName(SIDEngine value)
{
    switch (value) {
            
        case SIDENGINE_FASTSID:  return "FASTSID";
        case SIDENGINE_RESID:    return "RESID";
    }
    return "???";
}

// This enum reflects enum "sampling_method" used by reSID.
enum_long(SAMPLING)
{
    SAMPLING_FAST,
    SAMPLING_INTERPOLATE,
    SAMPLING_RESAMPLE,
    SAMPLING_RESAMPLE_FASTMEM
};
typedef SAMPLING SamplingMethod;

inline bool isSamplingMethod(long value)
{
    return (unsigned long)value <= SAMPLING_RESAMPLE_FASTMEM;
}

inline const char *SamplingMethodName(SamplingMethod method)
{
    switch (method) {
            
        case SAMPLING_FAST:              return "FAST";
        case SAMPLING_INTERPOLATE:       return "INTERPOLATE";
        case SAMPLING_RESAMPLE:          return "RESAMPLE";
        case SAMPLING_RESAMPLE_FASTMEM:  return "RESAMPLE FASTMEM";
    }
    return "???";
}

//
// Structures
//

typedef struct
{
    // Hardware settings
    SIDRevision revision;
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
