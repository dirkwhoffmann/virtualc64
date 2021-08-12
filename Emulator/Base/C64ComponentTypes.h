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
    // VICII
    OPT_VIC_REVISION,
    OPT_VIC_SPEED,
    OPT_PALETTE,
    OPT_BRIGHTNESS,
    OPT_CONTRAST,
    OPT_SATURATION,
    OPT_GRAY_DOT_BUG,
    OPT_VIC_POWER_SAVE,
    
    // Sprite debugger
    OPT_HIDE_SPRITES,
    OPT_CUT_LAYERS,
    OPT_CUT_OPACITY,
    OPT_SS_COLLISIONS,
    OPT_SB_COLLISIONS,

    // DMA Debugger
    OPT_DMA_DEBUG_ENABLE,
    OPT_DMA_DEBUG_MODE,
    OPT_DMA_DEBUG_COLOR,
    OPT_DMA_DEBUG_OPACITY,
    
    // Power supply
    OPT_POWER_GRID,
    
    // Logic board
    OPT_GLUE_LOGIC,

    // CIA
    OPT_CIA_REVISION,
    OPT_TIMER_B_BUG,
    
    // Mouse
    OPT_MOUSE_MODEL,
    OPT_SHAKE_DETECTION,
    OPT_MOUSE_VELOCITY,

    // Joystick
    OPT_AUTOFIRE,
    OPT_AUTOFIRE_BULLETS,
    OPT_AUTOFIRE_DELAY,
    
    // SID
    OPT_SID_ENABLE,
    OPT_SID_ADDRESS,
    OPT_SID_REVISION,
    OPT_SID_FILTER,
    OPT_SID_POWER_SAVE,
    OPT_SID_ENGINE,
    OPT_SID_SAMPLING,
    OPT_AUDPAN,
    OPT_AUDVOL,
    OPT_AUDVOLL,
    OPT_AUDVOLR,
        
    // Memory
    OPT_RAM_PATTERN,
    
    // Drive
    OPT_DRV_AUTO_CONFIG,
    OPT_DRV_TYPE,
    OPT_DRV_RAM,
    OPT_DRV_PARCABLE,
    OPT_DRV_CONNECT,
    OPT_DRV_POWER_SWITCH,
    OPT_DRV_POWER_SAVE,
    OPT_DRV_EJECT_DELAY,
    OPT_DRV_SWAP_DELAY,
    OPT_DRV_INSERT_DELAY,
    OPT_DRV_PAN,
    OPT_DRV_POWER_VOL,
    OPT_DRV_STEP_VOL,
    OPT_DRV_INSERT_VOL,
    OPT_DRV_EJECT_VOL,
            
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
                
            case OPT_VIC_REVISION:        return "VIC_REVISION";
            case OPT_PALETTE:             return "PALETTE";
            case OPT_BRIGHTNESS:          return "BRIGHTNESS";
            case OPT_CONTRAST:            return "CONTRAST";
            case OPT_SATURATION:          return "SATURATION";
            case OPT_GRAY_DOT_BUG:        return "GRAY_DOT_BUG";
            case OPT_VIC_POWER_SAVE:      return "VIC_POWER_SAVE";
                
            case OPT_HIDE_SPRITES:        return "HIDE_SPRITES";
            case OPT_CUT_LAYERS:          return "CUT_LAYERS";
            case OPT_CUT_OPACITY:         return "CUT_OPACITY";
            case OPT_SS_COLLISIONS:       return "SS_COLLISIONS";
            case OPT_SB_COLLISIONS:       return "SB_COLLISIONS";

            case OPT_DMA_DEBUG_ENABLE:    return "DMA_DEBUG";
            case OPT_DMA_DEBUG_MODE:      return "DMA_CHANNEL_R";
            case OPT_DMA_DEBUG_COLOR:     return "DMA_CHANNEL_I";
            case OPT_DMA_DEBUG_OPACITY:   return "DMA_CHANNEL_C";

            case OPT_POWER_GRID:          return "POWER_GRID";

            case OPT_GLUE_LOGIC:          return "GLUE_LOGIC";
                
            case OPT_CIA_REVISION:        return "CIA_REVISION";
            case OPT_TIMER_B_BUG:         return "TIMER_B_BUG";
                
            case OPT_MOUSE_MODEL:         return "MOUSE_MODEL";
            case OPT_SHAKE_DETECTION:     return "SHAKE_DETECTION";
            case OPT_MOUSE_VELOCITY:      return "MOUSE_VELOCITY";

            case OPT_AUTOFIRE:            return "AUTOFIRE";
            case OPT_AUTOFIRE_BULLETS:    return "AUTOFIRE_BULLETS";
            case OPT_AUTOFIRE_DELAY:      return "AUTOFIRE_DELAY";

            case OPT_SID_ENABLE:          return "SID_ENABLE";
            case OPT_SID_ADDRESS:         return "SID_ADDRESS";
            case OPT_SID_REVISION:        return "SID_REVISION";
            case OPT_SID_FILTER:          return "SID_FILTER";
            case OPT_SID_POWER_SAVE:      return "SID_POWER_SAVE";
            case OPT_SID_ENGINE:          return "SID_ENGINE";
            case OPT_SID_SAMPLING:        return "SID_SAMPLING";
            case OPT_AUDPAN:              return "AUDPAN";
            case OPT_AUDVOL:              return "AUDVOL";
            case OPT_AUDVOLL:             return "AUDVOLL";
            case OPT_AUDVOLR:             return "AUDVOLR";
                                
            case OPT_RAM_PATTERN:         return "RAM_PATTERN";
                
            case OPT_DRV_AUTO_CONFIG:     return "DRV_AUTO_CONFIG";
            case OPT_DRV_TYPE:            return "DRV_TYPE";
            case OPT_DRV_RAM:             return "OPT_DRV_RAM";
            case OPT_DRV_PARCABLE:        return "OPT_DRV_PARCABLE";
            case OPT_DRV_CONNECT:         return "DRV_CONNECT";
            case OPT_DRV_POWER_SWITCH:    return "DRV_POWER_SWITCH";
            case OPT_DRV_POWER_SAVE:      return "DRV_POWER_SAVE";
            case OPT_DRV_EJECT_DELAY:     return "DRV_EJECT_DELAY";
            case OPT_DRV_SWAP_DELAY:      return "DRV_SWAP_DELAY";
            case OPT_DRV_INSERT_DELAY:    return "DRV_INSERT_DELAY";
            case OPT_DRV_PAN:             return "DRV_PAN";
            case OPT_DRV_POWER_VOL:       return "DRV_POWER_VOL";
            case OPT_DRV_STEP_VOL:        return "DRV_STEP_VOL";
            case OPT_DRV_INSERT_VOL:      return "DRV_INSERT_VOL";
            case OPT_DRV_EJECT_VOL:       return "DRV_EJECT_VOL";
                                
            case OPT_COUNT:               return "???";
        }
        return "???";
    }
};
#endif
