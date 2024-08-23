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

#include "Reflection.h"
#include "C64Types.h"
#include "TODTypes.h"

namespace vc64 {

/// Chip revision
enum_long(CIAREV)
{
    MOS_6526,
    MOS_8521
};
typedef CIAREV CIARevision;

struct CIARevisionEnum : util::Reflection<CIARevisionEnum, CIARevision> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = MOS_8521;

    static const char *prefix() { return nullptr; }
    static const char *_key(long value)
    {
        switch (value) {

            case MOS_6526:      return "MOS_6526";
            case MOS_8521:      return "MOS_8521";
        }
        return "";
    }
};

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

struct CIARegEnum : util::Reflection<CIARegEnum, CIAReg> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = CIAREG_CRB;

    static const char *prefix() { return "CIAREG"; }
    static const char *_key(long value)
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


//
// Structures
//

typedef struct
{
    CIARevision revision;
    bool timerBBug;
    bool idleSleep;
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
}
CIAInfo;

typedef struct
{
    Cycle idleSince;
    Cycle idleTotal;
    double idlePercentage;
}
CIAStats;

}
