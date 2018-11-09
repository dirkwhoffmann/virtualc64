/*!
 * @header      TOD_types.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   Dirk W. Hoffmann, all rights reserved.
 */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

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
    uint32_t value;
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
