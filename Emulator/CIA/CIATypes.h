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
#include "TODTypes.h"

enum_long(CIAREV)
{
    MOS_6526,
    MOS_8521
};
typedef CIAREV CIARevision;

#ifdef __cplusplus
struct CIARevisionEnum : util::Reflection<CIARevisionEnum, CIARevision> {
    
    static long min() { return 0; }
    static long max() { return MOS_8521; }
    static bool isValid(long value) { return value >= min() && value <= max(); }

    static const char *prefix() { return nullptr; }
    static const char *key(CIARevision value)
    {
        switch (value) {
                
            case MOS_6526:      return "MOS_6526";
            case MOS_8521:      return "MOS_8521";
        }
        return "";
    }
};
#endif

enum_long(CIAREG)
{
    CIAREG_PRA,
    CIAREG_PRB,
    CIAREG_DDRA,
    CIAREG_DDRB,
    CIAREG_TALO,
    CIAREG_TAHI,
    CIAREG_TBLO,
    CIAREG_TBHI,
    CIAREG_TODTHS,
    CIAREG_TODSEC,
    CIAREG_TODMIN,
    CIAREG_TODHR,
    CIAREG_SDR,
    CIAREG_ICR,
    CIAREG_CRA,
    CIAREG_CRB
};
typedef CIAREG CIAReg;

#ifdef __cplusplus
struct CIARegEnum : util::Reflection<CIARegEnum, CIAReg> {

    static long min() { return 0; }
    static long max() { return CIAREG_CRB; }
    static bool isValid(long value) { return value >= min() && value <= max(); }
    
    static const char *prefix() { return "CIAREG"; }
    static const char *key(CIAReg value)
    {
        switch (value) {
                
            case CIAREG_PRA:     return "PRA";
            case CIAREG_PRB:     return "PRB";
            case CIAREG_DDRA:    return "DDRA";
            case CIAREG_DDRB:    return "DDRB";
            case CIAREG_TALO:    return "TALO";
            case CIAREG_TAHI:    return "TAHI";
            case CIAREG_TBLO:    return "TBLO";
            case CIAREG_TBHI:    return "TBHI";
            case CIAREG_TODTHS:  return "TODTHS";
            case CIAREG_TODSEC:  return "TODSEC";
            case CIAREG_TODMIN:  return "TODMIN";
            case CIAREG_TODHR:   return "TODHR";
            case CIAREG_SDR:     return "SDR";
            case CIAREG_ICR:     return "ICR";
            case CIAREG_CRA:     return "CRA";
            case CIAREG_CRB:     return "CRB";
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
    CIARevision revision;
    bool timerBBug;
}
CIAConfig;

typedef struct
{
    u8 port;
    u8 reg;
    u8 dir;
}
CIAPortInfo;

typedef struct
{
    u16 count;
    u16 latch;
    bool running;
    bool toggle;
    bool pbout;
    bool oneShot;
}
CIATimerInfo;

typedef struct
{
    CIAPortInfo portA;
    CIAPortInfo portB;

    CIATimerInfo timerA;
    CIATimerInfo timerB;

    u8 sdr;
    u8 ssr;
    u8 icr;
    u8 imr;
    bool intLine;
    
    TODInfo tod;
    bool todIntEnable;
    
    Cycle idleSince;
    Cycle idleTotal;
    double idlePercentage;
}
CIAInfo;
