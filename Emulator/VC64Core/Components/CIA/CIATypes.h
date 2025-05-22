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
enum class CIARev : long
{
    MOS_6526,
    MOS_8521
};

struct CIARevisionEnum : util::Reflection<CIARevisionEnum, CIARev> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = long(CIARev::MOS_8521);

    static const char *_key(CIARev value)
    {
        switch (value) {

            case CIARev::MOS_6526:      return "MOS_6526";
            case CIARev::MOS_8521:      return "MOS_8521";
        }
        return "";
    }
    
    static const char *help(CIARev value)
    {
        return "";
    }
};

enum class CIAReg : long
{
    PRA,
    PRB,
    DDRA,
    DDRB,
    TALO,
    TAHI,
    TBLO,
    TBHI,
    TODTHS,
    TODSEC,
    TODMIN,
    TODHR,
    SDR,
    ICR,
    CRA,
    CRB
};

struct CIARegEnum : util::Reflection<CIARegEnum, CIAReg> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = long(CIAReg::CRB);

    static const char *_key(CIAReg value)
    {
        switch (value) {

            case CIAReg::PRA:     return "PRA";
            case CIAReg::PRB:     return "PRB";
            case CIAReg::DDRA:    return "DDRA";
            case CIAReg::DDRB:    return "DDRB";
            case CIAReg::TALO:    return "TALO";
            case CIAReg::TAHI:    return "TAHI";
            case CIAReg::TBLO:    return "TBLO";
            case CIAReg::TBHI:    return "TBHI";
            case CIAReg::TODTHS:  return "TODTHS";
            case CIAReg::TODSEC:  return "TODSEC";
            case CIAReg::TODMIN:  return "TODMIN";
            case CIAReg::TODHR:   return "TODHR";
            case CIAReg::SDR:     return "SDR";
            case CIAReg::ICR:     return "ICR";
            case CIAReg::CRA:     return "CRA";
            case CIAReg::CRB:     return "CRB";
        }
        return "???";
    }
    
    static const char *help(CIAReg value)
    {
        return "";
    }
};


//
// Structures
//

typedef struct
{
    CIARev revision;
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
    Cycle totalCycles;
    Cycle idleCycles;
    Cycle idleSince;
    
    double idlePercentage;
}
CIAStats;

}
