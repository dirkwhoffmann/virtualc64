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

enum_long(OPT)
{
    // C64
    OPT_CYCLE_LIMIT,
    
    // VICII
    OPT_VIC_REVISION,
    OPT_PALETTE,
    OPT_GRAY_DOT_BUG,
    OPT_HIDE_SPRITES,
    OPT_DMA_DEBUG,
    OPT_DMA_CHANNEL_R,
    OPT_DMA_CHANNEL_I,
    OPT_DMA_CHANNEL_C,
    OPT_DMA_CHANNEL_G,
    OPT_DMA_CHANNEL_P,
    OPT_DMA_CHANNEL_S,
    OPT_DMA_COLOR_R,
    OPT_DMA_COLOR_I,
    OPT_DMA_COLOR_C,
    OPT_DMA_COLOR_G,
    OPT_DMA_COLOR_P,
    OPT_DMA_COLOR_S,
    OPT_DMA_DISPLAY_MODE,
    OPT_DMA_OPACITY,
    OPT_CUT_LAYERS,
    OPT_CUT_OPACITY,
    OPT_SS_COLLISIONS,
    OPT_SB_COLLISIONS,

    // Logic board
    OPT_GLUE_LOGIC,

    // CIA
    OPT_CIA_REVISION,
    OPT_TIMER_B_BUG,
    
    // SID
    OPT_SID_ENABLE,
    OPT_SID_ADDRESS,
    OPT_SID_REVISION,
    OPT_SID_FILTER,
    OPT_AUDPAN,
    OPT_AUDVOL,
    OPT_AUDVOLL,
    OPT_AUDVOLR,
    
    // Sound synthesis
    OPT_SID_ENGINE,
    OPT_SID_SAMPLING,
    
    // Memory
    OPT_RAM_PATTERN,
    
    // Drive
    OPT_DRIVE_TYPE,
    OPT_DRIVE_CONNECT,
    OPT_DRIVE_POWER_SWITCH,
    OPT_DRIVE_PAN,
    OPT_STEP_VOLUME,
    OPT_POLL_VOLUME,
    OPT_INSERT_VOLUME,
    OPT_EJECT_VOLUME,
    
    // Debugging
    OPT_DEBUGCART,
    
    OPT_COUNT
};
typedef OPT Option;

#ifdef __cplusplus
struct OptionEnum : util::Reflection<OptionEnum, Option> {
    
    static bool isValid(long value)
    {
        return (unsigned long)value < OPT_COUNT;
    }

    static const char *prefix() { return "OPT"; }
    static const char *key(Option value)
    {
        switch (value) {
            case OPT_CYCLE_LIMIT:         return "CYCLE_LIMIT";
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
            case OPT_DRIVE_PAN:           return "DRIVE_PAN";
            case OPT_STEP_VOLUME:         return "STEP_VOLUME";
            case OPT_POLL_VOLUME:         return "POLL_VOLUME";
            case OPT_INSERT_VOLUME:       return "INSERT_VOLUME";
            case OPT_EJECT_VOLUME:        return "EJECT_VOLUME";
                
            case OPT_DEBUGCART:           return "DEBUGCART";

            case OPT_COUNT:               return "???";
        }
        return "???";
    }
};
#endif
