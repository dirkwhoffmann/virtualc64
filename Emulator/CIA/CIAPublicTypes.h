// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

//
// Enumerations
//

enum_long(CIAREV)
{
    MOS_6526,
    MOS_8521,
    CIAREV_COUNT
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
    CIAREG_CRB,
    CIAREG_COUNT
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
