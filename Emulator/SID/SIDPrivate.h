// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef SID_PRIVATE_H
#define SID_PRIVATE_H

//
// Reflection APIs
//

struct SIDRevisionEnum : Reflection<SIDRevisionEnum, SIDRevision> {
    
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

struct SIDEngineEnum : Reflection<SIDEngineEnum, SIDEngine> {
    
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

struct SamplingMethodEnum : Reflection<SamplingMethodEnum, SamplingMethod> {
    
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
