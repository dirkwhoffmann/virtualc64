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

enum_long(CIAREV)
{
    MOS_6526,
    MOS_8521,
    CIAREV_COUNT
};
typedef CIAREV CIARevision;

#ifdef __cplusplus
struct CIARevisionEnum : util::Reflection<CIARevisionEnum, CIARevision> {
    
    static bool isValid(long value)
    {
        return (unsigned long)value < CIAREV_COUNT;
    }

    static const char *prefix() { return nullptr; }
    static const char *key(CIARevision value)
    {
        switch (value) {
                
            case MOS_6526:      return "MOS_6526";
            case MOS_8521:      return "MOS_8521";
            case CIAREV_COUNT:  return "???";
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
    CIAREG_CRB,
    CIAREG_COUNT
};
typedef CIAREG CIAReg;

#ifdef __cplusplus
struct CIARegEnum : util::Reflection<CIARegEnum, CIAReg> {

    static bool isValid(long value)
    {
        return (unsigned long)value < CIAREG_COUNT;
    }
    
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
            case CIAREG_COUNT:   return "???";
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

typedef union
{
    struct {
        u8 tenth;
        u8 sec;
        u8 min;
        u8 hour;
    };
    u32 value;
}
TimeOfDay;

typedef struct
{
    TimeOfDay time;
    TimeOfDay latch;
    TimeOfDay alarm;
}
TODInfo;

typedef struct
{
    struct {
        u8 port;
        u8 reg;
        u8 dir;
    } portA;

    struct {
        u8 port;
        u8 reg;
        u8 dir;
    } portB;

    struct {
        u16 count;
        u16 latch;
        bool running;
        bool toggle;
        bool pbout;
        bool oneShot;
    } timerA;

    struct {
        u16 count;
        u16 latch;
        bool running;
        bool toggle;
        bool pbout;
        bool oneShot;
    } timerB;

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
