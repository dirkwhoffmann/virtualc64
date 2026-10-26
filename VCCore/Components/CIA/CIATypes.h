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

#include "BasicTypes.h"
#include "C64Types.h"
#include "TODTypes.h"

namespace vc64 {

/// Chip revision
enum class CIARev : long
{
    MOS_6526,
    MOS_8521
};

struct CIARevisionEnum : utl::Reflectable<CIARevisionEnum, CIARev> {

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

struct CIARegEnum : utl::Reflectable<CIARegEnum, CIAReg> {

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
    CIARev revision;                        ///< Chip revision
    bool timerBBug;                         ///< Emulate the Timer-B bug
    bool idleSleep;                         ///< Enter the idle state while the chip is not in use
}
CIAConfig;

typedef struct
{
    u8 port;                                ///< State of the port pins
    u8 reg;                                 ///< Port register (PRA or PRB)
    u8 dir;                                 ///< Data direction register (DDRA or DDRB)
}
CIAPortInfo;

typedef struct
{
    u16 count;                              ///< Current counter value
    u16 latch;                              ///< Latch value the counter is reloaded from
    bool running;                           ///< The timer is running
    bool toggle;                            ///< Toggle mode is on
    bool pbout;                             ///< The timer output appears on port B
    bool oneShot;                           ///< One-shot mode is on
}
CIATimerInfo;

typedef struct
{
    CIAPortInfo portA;                      ///< Port A
    CIAPortInfo portB;                      ///< Port B

    CIATimerInfo timerA;                    ///< Timer A
    CIATimerInfo timerB;                    ///< Timer B

    u8 sdr;                                 ///< Serial data register (SDR)
    u8 ssr;                                 ///< Serial shift register
    u8 icr;                                 ///< Interrupt control register (pending interrupts)
    u8 imr;                                 ///< Interrupt mask register
    bool intLine;                           ///< The interrupt line is active

    TODInfo tod;                            ///< Time-of-day clock
    bool todIntEnable;                      ///< The TOD alarm interrupt is enabled
}
CIAInfo;

typedef struct
{
    Cycle totalCycles;                      ///< Total number of executed cycles
    Cycle idleCycles;                       ///< Number of cycles spent in the idle state
    Cycle idleSince;                        ///< Cycle at which the idle state started
    
    double idlePercentage;                  ///< Share of idle cycles
}
CIAMetrics;

}
