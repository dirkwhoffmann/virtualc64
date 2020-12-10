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

longenum(SIDRevision)
{
    MOS_6581,
    MOS_8580
};

inline bool isSIDRevision(long value)
{
    return value == MOS_6581 || value == MOS_8580;
}

inline const char *sidRevisionName(SIDRevision type)
{
    assert(isSIDRevision(type));
    
    switch (type) {
        case MOS_6581: return "MOS_6581";
        case MOS_8580: return "MOS_8580";
        default:       return "???";
    }
}

longenum(SIDEngine)
{
    ENGINE_FASTSID,
    ENGINE_RESID
};

inline bool isAudioEngine(long value)
{
    return value >= ENGINE_FASTSID && value <= ENGINE_RESID;
}

inline const char *sidEngineName(SIDEngine engine)
{
    assert(isAudioEngine(engine));
    
    switch (engine) {
        case ENGINE_FASTSID: return "FASTSID";
        case ENGINE_RESID:   return "RESID";
        default:             return "???";
    }
}

// This enum reflects enum "sampling_method" used by reSID.
longenum(SamplingMethod)
{
    SID_SAMPLE_FAST,
    SID_SAMPLE_INTERPOLATE,
    SID_SAMPLE_RESAMPLE,
    SID_SAMPLE_RESAMPLE_FASTMEM
};

inline bool isSamplingMethod(long value)
{
    return value >= SID_SAMPLE_FAST && value <= SID_SAMPLE_RESAMPLE_FASTMEM;
}

inline const char *sidSamplingMethodName(SamplingMethod method)
{
    assert(isSamplingMethod(method));
    
    switch (method) {
        case SID_SAMPLE_FAST:             return "FAST";
        case SID_SAMPLE_INTERPOLATE:      return "INTERPOLATE";
        case SID_SAMPLE_RESAMPLE:         return "RESAMPLE";
        case SID_SAMPLE_RESAMPLE_FASTMEM: return "RESAMPLE FASTMEM";
        default:                          return "???";
    }
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
    
    // Input channel volumes and pan settings
    long vol[4];
    long pan[4];
    
    // Output channel volumes
    long volL;
    long volR;

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
