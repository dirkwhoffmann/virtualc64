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
    SLOT_CIA1,                      // CIA 1 execution
    SLOT_CIA2,                      // CIA 2 execution
    SLOT_SEC,                       // Enables secondary slots

    // Secondary slots
    SLOT_DAT,                       // Datasette
    SLOT_TER,                       // Enables tertiary slots

    // Tertiary slots
    SLOT_DC8,                       // Disk change (Drive 8)
    SLOT_DC9,                       // Disk change (Drive 9)
    SLOT_KEY,                       // Auto-typing
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

            case SLOT_CIA1:  return "CIA1";
            case SLOT_CIA2:  return "CIA2";
            case SLOT_SEC:   return "SEC";

            case SLOT_DAT:   return "DAT";
            case SLOT_TER:   return "TER";

            case SLOT_DC8:   return "DC8";
            case SLOT_DC9:   return "DC9";
            case SLOT_KEY:   return "KEY";
            case SLOT_INS:   return "INS";

            case SLOT_COUNT: return "???";
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

    // Disk change slot
    DCH_INSERT          = 1,
    DCH_EJECT,
    DCH_EVENT_COUNT,

    // Auto typing
    KEY_PRESS           = 1,
    KEY_RELEASE,
    KEY_EVENT_COUNT,

    // Inspector slot
    INS_C64             = 1,
    INS_CPU,
    INS_MEM,
    INS_CIA,
    INS_VIC,
    INS_SID,
    INS_PORTS,
    INS_EVENTS,
    INS_EVENT_COUNT
};

enum_long(FPS_MODE)
{
    FPS_NATIVE,
    FPS_CUSTOM,
    FPS_VSYNC
};
typedef FPS_MODE FpsMode;

#ifdef __cplusplus
struct FpsModeEnum : util::Reflection<FpsModeEnum, FpsMode>
{
    static constexpr long minVal = 0;
    static constexpr long maxVal = FPS_VSYNC;
    static bool isValid(auto val) { return val >= minVal && val <= maxVal; }

    static const char *prefix() { return "FPS"; }
    static const char *key(FpsMode value)
    {
        switch (value) {

            case FPS_NATIVE:    return "NATIVE";
            case FPS_CUSTOM:    return "CUSTOM";
            case FPS_VSYNC:     return "VSYNC";
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
    INSPECTION_SID
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
    FpsMode fpsMode;
    isize fps;
}
C64Config;


//
// Private data types
//

#ifdef __cplusplus

typedef u32 RunLoopFlags;

namespace RL
{
constexpr u32 STOP          = 0b0000000001;
constexpr u32 INSPECT       = 0b0000000010;
constexpr u32 WARP_ON       = 0b0000000100;
constexpr u32 WARP_OFF      = 0b0000001000;
constexpr u32 BREAKPOINT    = 0b0000010000;
constexpr u32 WATCHPOINT    = 0b0000100000;
constexpr u32 AUTO_SNAPSHOT = 0b0001000000;
constexpr u32 USER_SNAPSHOT = 0b0010000000;
constexpr u32 CPU_JAM       = 0b0100000000;
constexpr u32 EXTERNAL_NMI  = 0b1000000000;
};

#endif
