// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// This FILE is dual-licensed. You are free to choose between:
//
//     - The GNU General Public License v3 (or any later version)
//     - The Mozilla Public License v2
//
// SPDX-License-Identifier: GPL-3.0-or-later OR MPL-2.0
// -----------------------------------------------------------------------------

#pragma once

#include "Aliases.h"
#include "Reflection.h"

//
// Enumerations
//

enum_long(EMULATOR_STATE)
{
    STATE_OFF,
    STATE_PAUSED,
    STATE_RUNNING,
    STATE_SUSPENDED,
    STATE_HALTED
};
typedef EMULATOR_STATE EmulatorState;

#ifdef __cplusplus
struct EmulatorStateEnum : util::Reflection<EmulatorStateEnum, EmulatorState>
{
    static constexpr long minVal = 0;
    static constexpr long maxVal = STATE_HALTED;
    static bool isValid(auto val) { return val >= minVal && val <= maxVal; }

    static const char *prefix() { return "STATE"; }
    static const char *key(EmulatorState value)
    {
        switch (value) {

            case STATE_OFF:          return "OFF";
            case STATE_PAUSED:       return "PAUSED";
            case STATE_RUNNING:      return "RUNNING";
            case STATE_SUSPENDED:    return "SUSPENDED";
            case STATE_HALTED:       return "HALTED";
        }
        return "???";
    }
};
#endif

enum_long(THREAD_TRANSITION)
{
    TRANSITION_POWER_OFF,
    TRANSITION_POWER_ON,
    TRANSITION_PAUSE,
    TRANSITION_RUN,
    TRANSITION_SUSPEND,
    TRANSITION_RESUME,
    TRANSITION_HALT,
    TRANSITION_WARP_ON,
    TRANSITION_WARP_OFF,
    TRANSITION_TRACK_ON,
    TRANSITION_TRACK_OFF
};
typedef THREAD_TRANSITION ThreadTransition;

#ifdef __cplusplus
struct ThreadTransitionEnum : util::Reflection<ThreadTransitionEnum, ThreadTransition>
{
    static constexpr long minVal = 0;
    static constexpr long maxVal = TRANSITION_TRACK_OFF;
    static bool isValid(auto val) { return val >= minVal && val <= maxVal; }

    static const char *prefix() { return "TRANSITION"; }
    static const char *key(ThreadTransition value)
    {
        switch (value) {

            case TRANSITION_POWER_OFF:  return "POWER_OFF";
            case TRANSITION_POWER_ON:   return "POWER_ON";
            case TRANSITION_PAUSE:      return "PAUSE";
            case TRANSITION_RUN:        return "RUN";
            case TRANSITION_SUSPEND:    return "SUSPEND";
            case TRANSITION_RESUME:     return "RESUME";
            case TRANSITION_HALT:       return "HALT";
            case TRANSITION_WARP_ON:    return "WARP_ON";
            case TRANSITION_WARP_OFF:   return "WARP_OFF";
            case TRANSITION_TRACK_ON:   return "TRACK_ON";
            case TRANSITION_TRACK_OFF:  return "TRACK_OFF";
        }
        return "???";
    }
};
#endif
