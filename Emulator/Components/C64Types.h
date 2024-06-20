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

#include "Reflection.h"

namespace vc64 {

//
// Type aliases
//

typedef i64 Cycle;


//
// Enumerations
//

/// C64 model
enum_long(C64_MODEL)
{
    C64_MODEL_PAL,      ///< C64 (PAL)
    C64_MODEL_PAL_II,   ///< C64 II (PAL)
    C64_MODEL_PAL_OLD,  ///< Early C64 (PAL)
    C64_MODEL_NTSC,     ///< C64 (NTSC)
    C64_MODEL_NTSC_II,  ///< C64 II (NTSC)
    C64_MODEL_NTSC_OLD  ///< Early C64 (NTSC)
};
typedef C64_MODEL C64Model;

struct C64ModelEnum : util::Reflection<C64ModelEnum, C64Model> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = C64_MODEL_NTSC_OLD;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return "C64_MODEL"; }
    static const char *key(long value)
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

enum_long(WARP_MODE)
{
    WARP_AUTO,
    WARP_NEVER,
    WARP_ALWAYS
};
typedef WARP_MODE WarpMode;

struct WarpModeEnum : util::Reflection<WarpModeEnum, WarpMode>
{
    static constexpr long minVal = 0;
    static constexpr long maxVal = WARP_ALWAYS;
    static bool isValid(auto val) { return val >= minVal && val <= maxVal; }

    static const char *prefix() { return "WARP"; }
    static const char *key(long value)
    {
        switch (value) {

            case WARP_AUTO:     return "WARP_AUTO";
            case WARP_NEVER:    return "WARP_NEVER";
            case WARP_ALWAYS:   return "WARP_ALWAYS";
        }
        return "???";
    }
};

enum_long(SLOT)
{
    // Primary slots
    SLOT_CIA1,                      // CIA 1 execution
    SLOT_CIA2,                      // CIA 2 execution
    SLOT_SEC,                       // Enables secondary slots

    // Secondary slots
    SLOT_SER,                       // Serial port (IEC bus)
    SLOT_DAT,                       // Datasette execution
    SLOT_TER,                       // Enables tertiary slots

    // Tertiary slots
    SLOT_AFI1,                      // Auto-fire (joystick port 1)
    SLOT_AFI2,                      // Auto-fire (joystick port 2)
    SLOT_MOT,                       // Datasette motor
    SLOT_DC8,                       // Disk change (Drive 8)
    SLOT_DC9,                       // Disk change (Drive 9)
    SLOT_SNP,                       // Snapshots
    SLOT_RSH,                       // Retro Shell
    SLOT_KEY,                       // Auto-typing
    SLOT_ALA,                       // Alarms (set by the GUI)
    SLOT_INS,                       // Handles periodic calls to inspect()

    SLOT_COUNT
};
typedef SLOT EventSlot;

struct EventSlotEnum : util::Reflection<EventSlotEnum, EventSlot>
{
    static constexpr long minVal = 0;
    static constexpr long maxVal = SLOT_COUNT - 1;
    static bool isValid(auto val) { return val >= minVal && val <= maxVal; }

    static const char *prefix() { return "SLOT"; }
    static const char *key(long value)
    {
        switch (value) {

            case SLOT_CIA1:     return "CIA1";
            case SLOT_CIA2:     return "CIA2";
            case SLOT_SEC:      return "SEC";

            case SLOT_SER:      return "SER";
            case SLOT_DAT:      return "DAT";
            case SLOT_TER:      return "TER";

            case SLOT_DC8:      return "DC8";
            case SLOT_DC9:      return "DC9";
            case SLOT_SNP:      return "SNP";
            case SLOT_RSH:      return "RSH";
            case SLOT_KEY:      return "KEY";
            case SLOT_ALA:      return "ALA";
            case SLOT_INS:      return "INS";

            case SLOT_COUNT:    return "???";
        }
        return "???";
    }
};

enum_i8(EventID)
{
    EVENT_NONE          = 0,

    //
    // Events in the primary event table
    //

    // CIA slots
    CIA_EXECUTE         = 1,
    CIA_WAKEUP,
    CIA_EVENT_COUNT,

    // SEC slot
    SEC_TRIGGER         = 1,
    SEC_EVENT_COUNT,

    //
    // Events in secondary event table
    //

    // SER slot
    SER_UPDATE          = 1,
    SER_EVENT_COUNT,

    // DAT slot
    DAT_EXECUTE         = 1,
    DAT_EVENT_COUNT,

    // SEC slot
    TER_TRIGGER         = 1,
    TER_EVENT_COUNT,

    //
    // Events in tertiary event table
    //

    // Auto-fire
    AFI_FIRE            = 1,
    AFI_EVENT_COUNT,

    // Datasette motor
    MOT_START           = 1,
    MOT_STOP,
    MOT_EVENT_COUNT,

    // Disk change
    DCH_INSERT          = 1,
    DCH_EJECT,
    DCH_EVENT_COUNT,

    // Snapshots
    SNP_TAKE             = 1,
    SNP_EVENT_COUNT,

    // Retro shell
    RSH_WAKEUP          = 1,
    RSH_EVENT_COUNT,

    // Auto typing
    KEY_AUTO_TYPE       = 1,
    KEY_EVENT_COUNT,

    // Alarm event slot
    ALA_TRIGGER         = 1,
    ALA_EVENT_COUNT,

    // Inspector slot
    INS_C64             = 1,
    INS_CPU,
    INS_MEM,
    INS_CIA,
    INS_VICII,
    INS_SID,
    INS_EVENTS,
    INS_EVENT_COUNT
};

enum_long(INSPECTION_TARGET)
{
    INSPECTION_NONE,
    INSPECTION_C64,
    INSPECTION_CPU,
    INSPECTION_CIA,
    INSPECTION_MEM,
    INSPECTION_VICII,
    INSPECTION_SID,
    INSPECTION_EVENTS

};
typedef INSPECTION_TARGET InspectionTarget;

struct InspectionTargetEnum : util::Reflection<InspectionTargetEnum, InspectionTarget> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = INSPECTION_EVENTS;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return "INSPECTION"; }
    static const char *key(long value)
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

//
// Structures
//

typedef struct
{
    //! After a reset, the emulator runs in warp mode for this amout of seconds
    isize warpBoot;

    //! Warp mode
    WarpMode warpMode;

    //! Vertical Synchronization
    bool vsync;

    //! Emulator speed in percent (100 is native speed)
    isize speedAdjust;

    //! Enable auto-snapshots
    bool snapshots;

    //! Delay between two auto-snapshots in seconds
    isize snapshotDelay;

    //! Number of run-ahead frames (0 = run-ahead is disabled)
    isize runAhead;
}
C64Config;

typedef struct
{
    u32 crc32;
    u64 fnv64;

    const char *title;
    const char *subtitle;
    const char *revision;

    bool isCommodoreRom;
    bool isPatchedRom;
    bool isMega65Rom;
}
RomInfo;

typedef struct
{
    Cycle cpuProgress;
    Cycle cia1Progress;
    Cycle cia2Progress;
    i64 frame;
    long vpos;
    long hpos;
}
C64Info;

typedef struct
{
    EventSlot slot;
    EventID eventId;
    const char *eventName;

    // Trigger cycle of the event
    Cycle trigger;
    Cycle triggerRel;

    // Trigger frame relative to the current frame
    long frameRel;

    // The trigger cycle translated to a beam position.
    long vpos;
    long hpos;
}
EventSlotInfo;

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

}
