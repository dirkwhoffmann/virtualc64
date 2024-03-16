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
/// @file

#pragma once

#include "Reflection.hpp"

// namespace vc64 {

#include "C64Types.h"

struct C64ModelEnum : util::Reflection<C64ModelEnum, C64Model> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = C64_MODEL_NTSC_OLD;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return "C64_MODEL"; }
    static const char *key(C64Model value)
    {
        switch (value) {

            case C64_MODEL_PAL:       return "PAL";
            case C64_MODEL_PAL_II:    return "PAL_II";
            case C64_MODEL_PAL_OLD:   return "PAL_OLD";
            case C64_MODEL_NTSC:      return "NTSC";
            case C64_MODEL_NTSC_II:   return "NTSC_II";
            case C64_MODEL_NTSC_OLD:  return "NTSC_OLD";
        }
        return "???";
    }
};

struct EventSlotEnum : util::Reflection<EventSlotEnum, EventSlot>
{
    static constexpr long minVal = 0;
    static constexpr long maxVal = SLOT_COUNT - 1;
    static bool isValid(auto val) { return val >= minVal && val <= maxVal; }

    static const char *prefix() { return "SLOT"; }
    static const char *key(EventSlot value)
    {
        switch (value) {

            case SLOT_CIA1:     return "CIA1";
            case SLOT_CIA2:     return "CIA2";
            case SLOT_SEC:      return "SEC";

            case SLOT_IEC:      return "IEC";
            case SLOT_DAT:      return "DAT";
            case SLOT_TER:      return "TER";

            case SLOT_DC8:      return "DC8";
            case SLOT_DC9:      return "DC9";
            case SLOT_RSH:      return "RSH";
            case SLOT_KEY:      return "KEY";
            case SLOT_ALA:      return "ALA";
            case SLOT_INS:      return "INS";

            case SLOT_COUNT:    return "???";
        }
        return "???";
    }
};

struct InspectionTargetEnum : util::Reflection<InspectionTargetEnum, InspectionTarget> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = INSPECTION_EVENTS;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return "INSPECTION"; }
    static const char *key(InspectionTarget value)
    {
        switch (value) {

            case INSPECTION_NONE:   return "NONE";
            case INSPECTION_C64:    return "C64";
            case INSPECTION_CPU:    return "CPU";
            case INSPECTION_CIA:    return "CIA";
            case INSPECTION_MEM:    return "MEM";
            case INSPECTION_VICII:  return "VICII";
            case INSPECTION_SID:    return "SID";
            case INSPECTION_EVENTS: return "EVENTS";
        }
        return "???";
    }
};

typedef u32 RunLoopFlags;

namespace RL {

constexpr u32 STOP          = (1 << 0);
constexpr u32 WARP_ON       = (1 << 1);
constexpr u32 WARP_OFF      = (1 << 2);
constexpr u32 BREAKPOINT    = (1 << 3);
constexpr u32 WATCHPOINT    = (1 << 4);
constexpr u32 CPU_JAM       = (1 << 5);
constexpr u32 SINGLE_STEP   = (1 << 6);
}

// }
