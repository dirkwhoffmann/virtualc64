/// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "Aliases.h"
#include "Reflection.h"

enum class ThreadMode { Periodic, Pulsed };

//
// Enumerations
//

enum_long(THREAD_STATE)
{
    THREAD_OFF,
    THREAD_PAUSED,
    THREAD_RUNNING,
    THREAD_TERMINATED
};
typedef THREAD_STATE ThreadState;

#ifdef __cplusplus
struct ThreadStateEnum : util::Reflection<ThreadStateEnum, ThreadState> {
    
    static bool isValid(long value)
    {
        return (unsigned long)value <= THREAD_TERMINATED;
    }

    static const char *prefix() { return "THREAD"; }
    static const char *key(ThreadState value)
    {
        switch (value) {
                
            case THREAD_OFF:         return "OFF";
            case THREAD_PAUSED:      return "PAUSED";
            case THREAD_RUNNING:     return "RUNNING";
            case THREAD_TERMINATED:  return "TERMINATED";
        }
        return "???";
    }
};
#endif
