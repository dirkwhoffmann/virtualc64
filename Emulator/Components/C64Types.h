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

enum_long(C64_MODEL)
{
    C64_MODEL_PAL,
    C64_MODEL_PAL_II,
    C64_MODEL_PAL_OLD,
    C64_MODEL_NTSC,
    C64_MODEL_NTSC_II,
    C64_MODEL_NTSC_OLD
};
typedef C64_MODEL C64Model;

#ifdef __cplusplus
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
#endif

enum_long(SLOT)
{
    // Primary slots
    SLOT_CIAA,                      // CIA 1 execution
    SLOT_CIAB,                      // CIA 2 execution
    SLOT_SEC,                       // Enables secondary slots

    // Secondary slots
    SLOT_DAT,                       // Datasette execution
    SLOT_TER,                       // Enables tertiary slots

    // Tertiary slots
    SLOT_MOT,                       // Datasette motor
    SLOT_DC8,                       // Disk change (Drive 8)
    SLOT_DC9,                       // Disk change (Drive 9)
    SLOT_RSH,                       // Retro Shell
    SLOT_KEY,                       // Auto-typing
    SLOT_WBT,                       // Warp boot
    SLOT_ALA,                       // Alarms (set by the GUI)
    SLOT_INS,                       // Handles periodic calls to inspect()

    SLOT_COUNT
};
typedef SLOT EventSlot;

#ifdef __cplusplus
struct EventSlotEnum : util::Reflection<EventSlotEnum, EventSlot>
{
    static constexpr long minVal = 0;
    static constexpr long maxVal = SLOT_COUNT - 1;
    static bool isValid(auto val) { return val >= minVal && val <= maxVal; }

    static const char *prefix() { return "SLOT"; }
    static const char *key(EventSlot value)
    {
        switch (value) {

            case SLOT_CIAA:     return "CIAA";
            case SLOT_CIAB:     return "CIAB";
            case SLOT_SEC:      return "SEC";

            case SLOT_DAT:      return "DAT";
            case SLOT_TER:      return "TER";

            case SLOT_DC8:      return "DC8";
            case SLOT_DC9:      return "DC9";
            case SLOT_RSH:      return "RSH";
            case SLOT_KEY:      return "KEY";
            case SLOT_WBT:      return "WBT";
            case SLOT_ALA:      return "ALA";
            case SLOT_INS:      return "INS";

            case SLOT_COUNT:    return "???";
        }
        return "???";
    }
};
#endif

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

    // DAT slot
    DAT_EXECUTE         = 1,
    DAT_EVENT_COUNT,

    // SEC slot
    TER_TRIGGER         = 1,
    TER_EVENT_COUNT,

    //
    // Events in tertiary event table
    //

    // Datasette motor
    MOT_START           = 1,
    MOT_STOP,

    // Disk change
    DCH_INSERT          = 1,
    DCH_EJECT,
    DCH_EVENT_COUNT,

    // Retro shell
    RSH_WAKEUP          = 1,
    RSH_EVENT_COUNT,
    
    // Auto typing
    KEY_AUTO_TYPE       = 1,
    KEY_EVENT_COUNT,

    // Warp boot
    WBT_DISABLE         = 1,
    WBT_EVENT_COUNT,

    // Alarm event slot
    ALA_TRIGGER         = 1,
    ALA_EVENT_COUNT,

    // Inspector slot
    INS_C64             = 1,
    INS_CPU,
    INS_MEM,
    INS_CIA,
    INS_VIC,
    INS_SID,
    INS_EVENTS,
    INS_EVENT_COUNT
};

enum_long(WARP_MODE)
{
    WARP_AUTO,
    WARP_NEVER,
    WARP_ALWAYS
};
typedef WARP_MODE WarpMode;

#ifdef __cplusplus
struct WarpModeEnum : util::Reflection<WarpModeEnum, WarpMode>
{
    static constexpr long minVal = 0;
    static constexpr long maxVal = WARP_ALWAYS;
    static bool isValid(auto val) { return val >= minVal && val <= maxVal; }

    static const char *prefix() { return "WARP"; }
    static const char *key(WarpMode value)
    {
        switch (value) {

            case WARP_AUTO:     return "WARP_AUTO";
            case WARP_NEVER:    return "WARP_NEVER";
            case WARP_ALWAYS:   return "WARP_ALWAYS";
        }
        return "???";
    }
};
#endif

enum_long(SYNC_MODE)
{
    SYNC_NATIVE_FPS,
    SYNC_FIXED_FPS,
    SYNC_VSYNC
};
typedef SYNC_MODE SyncMode;

#ifdef __cplusplus
struct SyncModeEnum : util::Reflection<SyncModeEnum, SyncMode>
{
    static constexpr long minVal = 0;
    static constexpr long maxVal = SYNC_VSYNC;
    static bool isValid(auto val) { return val >= minVal && val <= maxVal; }

    static const char *prefix() { return "SYNC"; }
    static const char *key(SyncMode value)
    {
        switch (value) {

            case SYNC_NATIVE_FPS:   return "NATIVE_FPS";
            case SYNC_FIXED_FPS:    return "FIXED_FPS";
            case SYNC_VSYNC:        return "VSYNC";
        }
        return "???";
    }
};
#endif

enum_long(ROM_TYPE)
{
    ROM_TYPE_BASIC,
    ROM_TYPE_CHAR,
    ROM_TYPE_KERNAL,
    ROM_TYPE_VC1541
};
typedef ROM_TYPE RomType;

#ifdef __cplusplus
struct RomTypeEnum : util::Reflection<RomTypeEnum, RomType> {
    
    static constexpr long minVal = 0;
    static constexpr long maxVal = ROM_TYPE_VC1541;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return "ROM_TYPE"; }
    static const char *key(RomType value)
    {
        switch (value) {
                
            case ROM_TYPE_BASIC:   return "BASIC";
            case ROM_TYPE_CHAR:    return "CHAR";
            case ROM_TYPE_KERNAL:  return "KERNAL";
            case ROM_TYPE_VC1541:  return "VC1541";
        }
        return "???";
    }
};
#endif

enum_long(INSPECTION_TARGET)
{
    INSPECTION_NONE,
    INSPECTION_C64,
    INSPECTION_CPU,
    INSPECTION_CIA,
    INSPECTION_MEM,
    INSPECTION_VIC,
    INSPECTION_SID,
    INSPECTION_EVENTS

};
typedef INSPECTION_TARGET InspectionTarget;

#ifdef __cplusplus
struct InspectionTargetEnum : util::Reflection<InspectionTargetEnum, InspectionTarget> {
    
    static constexpr long minVal = 0;
    static constexpr long maxVal = INSPECTION_SID;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }
    
    static const char *prefix() { return "INSPECTION"; }
    static const char *key(InspectionTarget value)
    {
        switch (value) {
                
            case INSPECTION_NONE:  return "NONE";
            case INSPECTION_C64:   return "C64";
            case INSPECTION_CPU:   return "CPU";
            case INSPECTION_CIA:   return "CIA";
            case INSPECTION_MEM:   return "MEM";
            case INSPECTION_VIC:   return "VIC";
            case INSPECTION_SID:   return "SID";
        }
        return "???";
    }
};
#endif


//
// Structures
//

typedef struct
{
    isize warpBoot;
    WarpMode warpMode;
    SyncMode syncMode;
    isize proposedFps;
}
C64Config;

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

typedef struct
{
    Cycle cpuProgress;
    Cycle cia1Progress;
    Cycle cia2Progress;
    i64 frame;
    long vpos;
    long hpos;
}
EventInfo;


//
// Private data types
//

#ifdef __cplusplus

typedef u32 RunLoopFlags;

namespace RL
{
constexpr u32 STOP          = (1 << 0);
constexpr u32 INSPECT       = (1 << 1);
constexpr u32 WARP_ON       = (1 << 2);
constexpr u32 WARP_OFF      = (1 << 3);
constexpr u32 BREAKPOINT    = (1 << 4);
constexpr u32 WATCHPOINT    = (1 << 5);
constexpr u32 AUTO_SNAPSHOT = (1 << 6);
constexpr u32 USER_SNAPSHOT = (1 << 7);
constexpr u32 CPU_JAM       = (1 << 8);
constexpr u32 EXTERNAL_NMI  = (1 << 9);
constexpr u32 EXTERNAL_BRK  = (1 << 10);
constexpr u32 SYNC_THREAD   = (1 << 11);
};

#endif
