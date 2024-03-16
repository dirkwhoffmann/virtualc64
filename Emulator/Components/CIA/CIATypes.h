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

#include "Aliases.h"
#include "TODTypes.h"

/// Chip revision
enum_long(CIAREV)
{
    MOS_6526,
    MOS_8521
};
typedef CIAREV CIARevision;

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
}
CIAInfo;

typedef struct
{
    Cycle idleSince;
    Cycle idleTotal;
    double idlePercentage;
}
CIAStats;
