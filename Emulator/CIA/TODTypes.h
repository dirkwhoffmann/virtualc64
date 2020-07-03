// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef TOD_TYPES_H
#define TOD_TYPES_H

#include <stdint.h>

//! @brief    Time of day information (TOD)
typedef union {
    struct {
        u8 tenth;
        u8 seconds;
        u8 minutes;
        u8 hours;
    };
    u32 value;
} TimeOfDay;

/*! @brief    TOD info
 *  @details  Used by TOD::getInfo() to collect debug information
 */
typedef struct {
    TimeOfDay time;
    TimeOfDay latch;
    TimeOfDay alarm;
} TODInfo;

#endif
