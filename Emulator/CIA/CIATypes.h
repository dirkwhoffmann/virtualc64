// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _CIA_T_H
#define _CIA_T_H

#include "TODTypes.h"

typedef enum : long
{
    MOS_6526,
    MOS_8521
}
CIARevision;

inline bool
isCIARevision(long value)
{
    return value == MOS_6526 || value == MOS_8521;
}

inline const char *
ciaRevisionName(CIARevision type)
{
    assert(isCIARevision(type));
    
    switch (type) {
        case MOS_6526: return "MOS_6526";
        case MOS_8521: return "MOS_8521";
        default:       return "???";
    }
}

typedef struct
{
    CIARevision revision;
    bool timerBBug;
}
CIAConfig;

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
    u8 icr;
    u8 imr;
    bool intLine;
    TODInfo tod;
    bool todIntEnable;
}
CIAInfo;

#endif
