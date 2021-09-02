// -----------------------------------------------------------------------------
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

//
// Enumerations
//

enum_long(EXEC_STATE)
{
    EXEC_OFF,
    EXEC_PAUSED,
    EXEC_RUNNING,
    EXEC_HALTED
};
typedef EXEC_STATE ExecutionState;

#ifdef __cplusplus
struct ExecutionStateEnum : util::Reflection<ExecutionStateEnum, ExecutionState> {
    
    static long min() { return 0; }
    static long max() { return EXEC_HALTED; }
    static bool isValid(long value) { return value >= min() && value <= max(); }

    static const char *prefix() { return "EXEC"; }
    static const char *key(ExecutionState value)
    {
        switch (value) {
                
            case EXEC_OFF:         return "OFF";
            case EXEC_PAUSED:      return "PAUSED";
            case EXEC_RUNNING:     return "RUNNING";
            case EXEC_HALTED:      return "HALTED";
        }
        return "???";
    }
};
#endif
