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
    
    static bool isValid(long value)
    {
        return (unsigned long)value <= C64_MODEL_NTSC_OLD;
    }

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
    
    static bool isValid(long value)
    {
        return (unsigned long)value <= ROM_TYPE_VC1541;
    }

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

enum_long(EMULATOR_STATE)
{
    EMULATOR_STATE_OFF,
    EMULATOR_STATE_PAUSED,
    EMULATOR_STATE_RUNNING
};
typedef EMULATOR_STATE EmulatorState;

#ifdef __cplusplus
struct EmulatorStateEnum : util::Reflection<EmulatorStateEnum, EmulatorState> {
    
    static bool isValid(long value)
    {
        return (unsigned long)value <= EMULATOR_STATE_RUNNING;
    }

    static const char *prefix() { return "EMULATOR_STATE"; }
    static const char *key(EmulatorState value)
    {
        switch (value) {
                
            case EMULATOR_STATE_OFF:      return "OFF";
            case EMULATOR_STATE_PAUSED:   return "PAUSED";
            case EMULATOR_STATE_RUNNING:  return "RUNNING";
        }
        return "???";
    }
};
#endif

enum_long(INSPECTION_TARGET)
{
    INSPECTION_TARGET_NONE,
    INSPECTION_TARGET_CPU,
    INSPECTION_TARGET_MEM,
    INSPECTION_TARGET_CIA,
    INSPECTION_TARGET_VIC,
    INSPECTION_TARGET_SID
};
typedef INSPECTION_TARGET InspectionTarget;

#ifdef __cplusplus
struct InspectionTargetEnum : util::Reflection<InspectionTargetEnum, InspectionTarget> {
    
    static bool isValid(long value)
    {
        return (unsigned long)value <= INSPECTION_TARGET_SID;
    }
    
    static const char *prefix() { return "INSPECTION_TARGET"; }
    static const char *key(InspectionTarget value)
    {
        switch (value) {
                
            case INSPECTION_TARGET_NONE:  return "NONE";
            case INSPECTION_TARGET_CPU:   return "CPU";
            case INSPECTION_TARGET_MEM:   return "IMEM";
            case INSPECTION_TARGET_CIA:   return "CIA";
            case INSPECTION_TARGET_VIC:   return "VIC";
            case INSPECTION_TARGET_SID:   return "SID";
        }
        return "???";
    }
};
#endif


//
// Private data types
//

enum_long(ACTION_FLAG)
{
    ACTION_FLAG_STOP          = 0b000000001,
    ACTION_FLAG_CPU_JAM       = 0b000000010,
    ACTION_FLAG_EXTERNAL_NMI  = 0b000000100,
    ACTION_FLAG_INSPECT       = 0b000001000,
    ACTION_FLAG_BREAKPOINT    = 0b000010000,
    ACTION_FLAG_WATCHPOINT    = 0b000100000,
    ACTION_FLAG_AUTO_SNAPSHOT = 0b001000000,
    ACTION_FLAG_USER_SNAPSHOT = 0b010000000
};
typedef ACTION_FLAG ActionFlag;
