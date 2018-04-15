//
//  TOD_types.h
//  V64
//
//  Created by Dirk Hoffmann on 15.04.18.
//

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
