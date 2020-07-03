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
        uint8_t tenth;
        uint8_t seconds;
        uint8_t minutes;
        uint8_t hours;
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
