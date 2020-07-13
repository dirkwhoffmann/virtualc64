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

typedef enum : long
{
    MOS_6581,
    MOS_8580
}
SIDRevision;

inline bool
isSIDRevision(long value)
{
    return value == MOS_6581 || value == MOS_8580;
}

inline const char *
sidRevisionName(SIDRevision type)
{
    assert(isSIDRevision(type));
    
    switch (type) {
        case MOS_6581: return "MOS_6581";
        case MOS_8580: return "MOS_8580";
        default:       return "???";
    }
}

// This enum reflects enum "sampling_method" used by reSID.
typedef enum : long
{
    SID_SAMPLE_FAST,
    SID_SAMPLE_INTERPOLATE,
    SID_SAMPLE_RESAMPLE,
    SID_SAMPLE_RESAMPLE_FASTMEM
}
SamplingMethod;

inline bool
isSamplingMethod(long value)
{
    return value >= SID_SAMPLE_FAST && value <= SID_SAMPLE_RESAMPLE_FASTMEM;
}

inline const char *
sidSamplingMethodName(SamplingMethod method)
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
    SIDRevision revision;
}
SIDConfig;


/*! @brief    Voice info
 *  @details  Part of SIDInfo
 */
typedef struct {
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
} VoiceInfo;

/*! @brief    SID info
 *  @details  Used by SIDBridge::getInfo() to collect debug information
 */
typedef struct {
    u8 volume;
    u16 filterCutoff;
    u8 filterResonance;
    u8 filterModeBits;
    u8 filterType;
    u8 filterEnableBits;
    u8 potX;
    u8 potY;
} SIDInfo;

#endif
