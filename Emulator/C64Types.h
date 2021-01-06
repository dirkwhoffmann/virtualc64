// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

/* This file defines all constants and data types that are not exposed to the
 * GUI. It also defines several data type extensions such as Reflections
 * APIs for various enum data types.
 */

#pragma once

#include "C64PublicTypes.h"
#include "CartridgeTypes.h"
#include "CIATypes.h"
#include "CPUTypes.h"
#include "DriveTypes.h"
#include "DiskTypes.h"
#include "FileTypes.h"
#include "FSTypes.h"
#include "MemoryTypes.h"
#include "MsgQueueTypes.h"
#include "MouseTypes.h"
#include "PortTypes.h"
#include "SIDTypes.h"
#include "VICIITypes.h"

//
// Reflection APIs
//

struct OptionEnum : Reflection<OptionEnum, Option> {
    
    static bool isValid(long value)
    {
        return (unsigned long)value < OPT_COUNT;
    }

    static const char *prefix() { return "OPT"; }
    static const char *key(Option value)
    {
        switch (value) {
                
            case OPT_VIC_REVISION:        return "VIC_REVISION";
            case OPT_PALETTE:             return "PALETTE";
            case OPT_GRAY_DOT_BUG:        return "GRAY_DOT_BUG";
            case OPT_HIDE_SPRITES:        return "HIDE_SPRITES";
            case OPT_DMA_DEBUG:           return "DMA_DEBUG";
            case OPT_DMA_CHANNEL_R:       return "DMA_CHANNEL_R";
            case OPT_DMA_CHANNEL_I:       return "DMA_CHANNEL_I";
            case OPT_DMA_CHANNEL_C:       return "DMA_CHANNEL_C";
            case OPT_DMA_CHANNEL_G:       return "DMA_CHANNEL_G";
            case OPT_DMA_CHANNEL_P:       return "DMA_CHANNEL_P";
            case OPT_DMA_CHANNEL_S:       return "DMA_CHANNEL_S";
            case OPT_DMA_COLOR_R:         return "DMA_COLOR_R";
            case OPT_DMA_COLOR_I:         return "DMA_COLOR_I";
            case OPT_DMA_COLOR_C:         return "DMA_COLOR_C";
            case OPT_DMA_COLOR_G:         return "DMA_COLOR_G";
            case OPT_DMA_COLOR_P:         return "DMA_COLOR_P";
            case OPT_DMA_COLOR_S:         return "DMA_COLOR_S";
            case OPT_DMA_DISPLAY_MODE:    return "DMA_DISPLAY_MODE";
            case OPT_DMA_OPACITY:         return "DMA_OPACITY";
            case OPT_CUT_LAYERS:          return "CUT_LAYERS";
            case OPT_CUT_OPACITY:         return "CUT_OPACITY";
            case OPT_SS_COLLISIONS:       return "SS_COLLISIONS";
            case OPT_SB_COLLISIONS:       return "SB_COLLISIONS";
                
            case OPT_GLUE_LOGIC:          return "GLUE_LOGIC";
                
            case OPT_CIA_REVISION:        return "CIA_REVISION";
            case OPT_TIMER_B_BUG:         return "TIMER_B_BUG";
                
            case OPT_SID_ENABLE:          return "SID_ENABLE";
            case OPT_SID_ADDRESS:         return "SID_ADDRESS";
            case OPT_SID_REVISION:        return "SID_REVISION";
            case OPT_SID_FILTER:          return "SID_FILTER";
            case OPT_AUDPAN:              return "AUDPAN";
            case OPT_AUDVOL:              return "AUDVOL";
            case OPT_AUDVOLL:             return "AUDVOLL";
            case OPT_AUDVOLR:             return "AUDVOLR";
                
            case OPT_SID_ENGINE:          return "SID_ENGINE";
            case OPT_SID_SAMPLING:        return "SID_SAMPLING";
                
            case OPT_RAM_PATTERN:         return "RAM_PATTERN";
                
            case OPT_DRIVE_TYPE:          return "DRIVE_TYPE";
            case OPT_DRIVE_CONNECT:       return "DRIVE_CONNECT";
            case OPT_DRIVE_POWER_SWITCH:  return "DRIVE_POWER_SWITCH";
                
            case OPT_DEBUGCART:           return "DEBUGCART";

            case OPT_COUNT:               return "???";
        }
        return "???";
    }
};

struct C64ModelEnum : Reflection<C64ModelEnum, C64Model> {
    
    static bool isValid(long value)
    {
        return (unsigned long)value < C64_MODEL_COUNT;
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
            case C64_MODEL_CUSTOM:    return "CUSTOM";
            case C64_MODEL_COUNT:     return "???";
        }
        return "???";
    }
};

struct RomTypeEnum : Reflection<RomTypeEnum, RomType> {
    
    static bool isValid(long value)
    {
        return (unsigned long)value < ROM_TYPE_COUNT;
    }

    static const char *prefix() { return "ROM_TYPE"; }
    static const char *key(RomType value)
    {
        switch (value) {
                
            case ROM_TYPE_BASIC:   return "BASIC";
            case ROM_TYPE_CHAR:    return "CHAR";
            case ROM_TYPE_KERNAL:  return "KERNAL";
            case ROM_TYPE_VC1541:  return "VC1541";
            case ROM_TYPE_COUNT:   return "???";
        }
        return "???";
    }
};

struct EmulatorStateEnum : Reflection<EmulatorStateEnum, EmulatorState> {
    
    static bool isValid(long value)
    {
        return (unsigned long)value < EMULATOR_STATE_COUNT;
    }

    static const char *prefix() { return "EMULATOR_STATE"; }
    static const char *key(EmulatorState value)
    {
        switch (value) {
                
            case EMULATOR_STATE_OFF:      return "OFF";
            case EMULATOR_STATE_PAUSED:   return "PAUSED";
            case EMULATOR_STATE_RUNNING:  return "RUNNING";
            case EMULATOR_STATE_COUNT:    return "???";
        }
        return "???";
    }
};

struct InspectionTargetEnum : Reflection<InspectionTargetEnum, InspectionTarget> {
    
    static bool isValid(long value)
    {
        return (unsigned long)value < INSPECTION_TARGET_COUNT;
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
            case INSPECTION_TARGET_COUNT: return "???";
        }
        return "???";
    }
};

struct ErrorCodeEnum : Reflection<ErrorCodeEnum, ErrorCode> {
    
    static bool isValid(long value)
    {
        return (unsigned long)value < ERROR_COUNT;
    }
    
    static const char *prefix() { return "ERROR"; }
    static const char *key(ErrorCode value)
    {
        switch (value) {
                
            case ERROR_OK:                  return "OK";
                
            case ERROR_OUT_OF_MEMORY:       return "OUT_OF_MEMORY";
                
            case ERROR_FILE_NOT_FOUND:      return "FILE_NOT_FOUND";
            case ERROR_FILE_TYPE_MISMATCH:  return "FILE_TYPE_MISMATCH";
            case ERROR_FILE_CANT_READ:      return "FILE_CANT_READ";
            case ERROR_FILE_CANT_WRITE:     return "FILE_CANT_WRITE";
            case ERROR_FILE_CANT_CREATE:    return "FILE_CANT_CREATE";
            case ERROR_DIR_CANT_CREATE:     return "DIR_CANT_CREATE";
            case ERROR_DIR_NOT_EMPTY:       return "DIR_NOT_EMPTY";
                
            case ERROR_SNP_UNSUPPORTED:     return "SNP_UNSUPPORTED";
                
            case ERROR_CRT_UNSUPPORTED:     return "CRT_UNSUPPORTED";
                
            case ERROR_FS_UNSUPPORTED:      return "FS_UNSUPPORTED";
            case ERROR_FS_WRONG_CAPACITY:   return "FS_WRONG_CAPACITY";
            case ERROR_FS_CORRUPTED:        return "FS_CORRUPTED";
            case ERROR_FS_HAS_CYCLES:       return "FS_HAS_CYCLES";
            case ERROR_FS_CANT_IMPORT:      return "FS_CANT_IMPORT";
            case ERROR_FS_EXPECTED_VAL:     return "FS_EXPECTED_VAL";
            case ERROR_FS_EXPECTED_MIN:     return "FS_EXPECTED_MIN";
            case ERROR_FS_EXPECTED_MAX:     return "FS_EXPECTED_MAX";

            case ERROR_COUNT:               return "???";
        }
        return "???";
    }
};


//
// Private data types
//

enum_int(ACTION_FLAG)
{
    ACTION_FLAG_STOP          = 0b0000001,
    ACTION_FLAG_CPU_JAMMED    = 0b0000010,
    ACTION_FLAG_INSPECT       = 0b0000100,
    ACTION_FLAG_BREAKPOINT    = 0b0001000,
    ACTION_FLAG_WATCHPOINT    = 0b0010000,
    ACTION_FLAG_AUTO_SNAPSHOT = 0b0100000,
    ACTION_FLAG_USER_SNAPSHOT = 0b1000000
};
typedef ACTION_FLAG ActionFlag;
