// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _TOD_T_H
#define _TOD_T_H

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

#endif
