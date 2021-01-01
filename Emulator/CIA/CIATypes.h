// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _CIA_TYPES_H
#define _CIA_TYPES_H

//
// Enumerations
//

enum_long(CIARev)
{
    MOS_6526,
    MOS_8521
};

inline bool isCIARev(long value)
{
    return (unsigned long)value <= MOS_8521;
}

inline const char * CIARevisionName(CIARev value)
{    
    switch (value) {
            
        case MOS_6526:  return "MOS_6526";
        case MOS_8521:  return "MOS_8521";
        default:        return "???";
    }
}

//
// Structures
//

typedef struct
{
    CIARev revision;
    bool timerBBug;
}
CIAConfig;

typedef union
{
    struct {
        u8 tenth;
        u8 seconds;
        u8 minutes;
        u8 hours;
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

#endif
