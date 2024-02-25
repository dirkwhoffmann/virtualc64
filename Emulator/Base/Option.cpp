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

#include "config.h"
#include "Option.h"
#include "VICIITypes.h"
#include "DatasetteTypes.h"
#include "MemoryTypes.h"
#include "SIDTypes.h"
#include "CIATypes.h"
#include "DriveTypes.h"
#include "ParCableTypes.h"
#include "PowerSupplyTypes.h"
#include "MouseTypes.h"

namespace vc64 {

OptionParser
OptionParser::create(Option opt)
{
    switch (opt) {

        case OPT_EMU_WARP_MODE:             return EnumParser<VICIIRevisionEnum>(opt);
        case OPT_EMU_WARP_BOOT:             return BoolParser(opt);
        case OPT_EMU_VSYNC:                 return BoolParser(opt);
        case OPT_EMU_TIME_LAPSE:            return NumParser(opt);
        case OPT_EMU_RUN_AHEAD:             return NumParser(opt);

        case OPT_HOST_SAMPLE_RATE:          return NumParser(opt);

        case OPT_HOST_REFRESH_RATE:         return NumParser(opt);
        case OPT_HOST_FRAMEBUF_WIDTH:       return NumParser(opt);
        case OPT_HOST_FRAMEBUF_HEIGHT:      return NumParser(opt);

        case OPT_VICII_REVISION:            return EnumParser<VICIIRevisionEnum>(opt);
        case OPT_VICII_PALETTE:             return EnumParser<PaletteEnum>(opt);
        case OPT_VICII_BRIGHTNESS:          return NumParser(opt);
        case OPT_VICII_CONTRAST:            return NumParser(opt);
        case OPT_VICII_SATURATION:          return NumParser(opt);
        case OPT_VICII_GRAY_DOT_BUG:        return BoolParser(opt);
        case OPT_VICII_POWER_SAVE:          return BoolParser(opt);
        case OPT_VICII_HIDE_SPRITES:        return BoolParser(opt);
        case OPT_VICII_SS_COLLISIONS:       return BoolParser(opt);
        case OPT_VICII_SB_COLLISIONS:       return BoolParser(opt);
        case OPT_GLUE_LOGIC:                return EnumParser<GlueLogicEnum>(opt);

        case OPT_VICII_CUT_LAYERS:          return NumParser(opt);
        case OPT_VICII_CUT_OPACITY:         return NumParser(opt);
        case OPT_DMA_DEBUG_ENABLE:          return BoolParser(opt);
        case OPT_DMA_DEBUG_MODE:            return EnumParser<DmaDisplayModeEnum>(opt);
        case OPT_DMA_DEBUG_OPACITY:         return NumParser(opt);
        case OPT_DMA_DEBUG_CHANNEL0:        return BoolParser(opt);
        case OPT_DMA_DEBUG_CHANNEL1:        return BoolParser(opt);
        case OPT_DMA_DEBUG_CHANNEL2:        return BoolParser(opt);
        case OPT_DMA_DEBUG_CHANNEL3:        return BoolParser(opt);
        case OPT_DMA_DEBUG_CHANNEL4:        return BoolParser(opt);
        case OPT_DMA_DEBUG_CHANNEL5:        return BoolParser(opt);
        case OPT_DMA_DEBUG_COLOR0:          return NumParser(opt);
        case OPT_DMA_DEBUG_COLOR1:          return NumParser(opt);
        case OPT_DMA_DEBUG_COLOR2:          return NumParser(opt);
        case OPT_DMA_DEBUG_COLOR3:          return NumParser(opt);
        case OPT_DMA_DEBUG_COLOR4:          return NumParser(opt);
        case OPT_DMA_DEBUG_COLOR5:          return NumParser(opt);

        case OPT_POWER_GRID:                return EnumParser<PowerGridEnum>(opt);

        case OPT_CIA_REVISION:              return EnumParser<CIARevisionEnum>(opt);
        case OPT_CIA_TIMER_B_BUG:           return BoolParser(opt);

        case OPT_SID_ENABLE:                return NumParser(opt);
        case OPT_SID_ADDRESS:               return NumParser(opt);

        case OPT_SID_REVISION:              return EnumParser<SIDRevisionEnum>(opt);
        case OPT_SID_FILTER:                return BoolParser(opt);
        case OPT_SID_SAMPLING:              return EnumParser<SamplingMethodEnum>(opt);
        case OPT_SID_POWER_SAVE:            return BoolParser(opt);
        case OPT_SID_ENGINE:                return EnumParser<SIDEngineEnum>(opt);
        case OPT_AUD_PAN:                   return NumParser(opt);
        case OPT_AUD_VOL:                   return NumParser(opt);
        case OPT_AUD_VOL_L:                 return NumParser(opt);
        case OPT_AUD_VOL_R:                 return NumParser(opt);

        case OPT_RAM_PATTERN:               return EnumParser<RamPatternEnum>(opt);

        case OPT_SAVE_ROMS:                 return BoolParser(opt);

        case OPT_DRV_AUTO_CONFIG:           return BoolParser(opt);
        case OPT_DRV_TYPE:                  return EnumParser<DriveTypeEnum>(opt);
        case OPT_DRV_RAM:                   return EnumParser<DriveRamEnum>(opt);
        case OPT_DRV_PARCABLE:              return EnumParser<ParCableTypeEnum>(opt);
        case OPT_DRV_CONNECT:               return BoolParser(opt);
        case OPT_DRV_POWER_SWITCH:          return BoolParser(opt);
        case OPT_DRV_POWER_SAVE:            return BoolParser(opt);
        case OPT_DRV_EJECT_DELAY:           return NumParser(opt);
        case OPT_DRV_SWAP_DELAY:            return NumParser(opt);
        case OPT_DRV_INSERT_DELAY:          return NumParser(opt);
        case OPT_DRV_PAN:                   return NumParser(opt);
        case OPT_DRV_POWER_VOL:             return NumParser(opt);
        case OPT_DRV_STEP_VOL:              return NumParser(opt);
        case OPT_DRV_INSERT_VOL:            return NumParser(opt);
        case OPT_DRV_EJECT_VOL:             return NumParser(opt);

        case OPT_DAT_MODEL:                 return EnumParser<DatasetteModelEnum>(opt);
        case OPT_DAT_CONNECT:               return BoolParser(opt);

        case OPT_MOUSE_MODEL:               return EnumParser<MouseModelEnum>(opt);
        case OPT_MOUSE_SHAKE_DETECT:        return BoolParser(opt);
        case OPT_MOUSE_VELOCITY:            return NumParser(opt);

        case OPT_JOY_AUTOFIRE:              return BoolParser(opt);
        case OPT_JOY_AUTOFIRE_BULLETS:      return NumParser(opt);
        case OPT_JOY_AUTOFIRE_DELAY:        return NumParser(opt);

    }
    fatalError;
}

const char *
OptionParser::help()
{
    switch (opt) {

            /*
             case OPT_HOST_REFRESH_RATE:     return "HOST_REFRESH_RATE";
             case OPT_HOST_SAMPLE_RATE:      return "HOST_SAMPLE_RATE";
             case OPT_HOST_FRAMEBUF_WIDTH:   return "HOST_FRAMEBUF_WIDTH";
             case OPT_HOST_FRAMEBUF_HEIGHT:  return "HOST_FRAMEBUF_HEIGHT";

             case OPT_EMU_WARP_BOOT:         return "WARP_BOOT";
             case OPT_EMU_WARP_MODE:         return "WARP_MODE";
             case OPT_EMU_VSYNC:             return "VSYNC";
             case OPT_EMU_TIME_LAPSE:        return "TIME_LAPSE";
             case OPT_EMU_RUN_AHEAD:         return "RUN_AHEAD";

             case OPT_VICII_REVISION:        return "VIC_REVISION";
             case OPT_VICII_PALETTE:         return "PALETTE";
             case OPT_VICII_BRIGHTNESS:      return "BRIGHTNESS";
             case OPT_VICII_CONTRAST:        return "CONTRAST";
             case OPT_VICII_SATURATION:      return "SATURATION";
             case OPT_VICII_GRAY_DOT_BUG:    return "GRAY_DOT_BUG";
             case OPT_VICII_POWER_SAVE:      return "VIC_POWER_SAVE";

             case OPT_VICII_HIDE_SPRITES:    return "HIDE_SPRITES";
             case OPT_VICII_CUT_LAYERS:      return "CUT_LAYERS";
             case OPT_VICII_CUT_OPACITY:     return "CUT_OPACITY";
             case OPT_VICII_SS_COLLISIONS:   return "SS_COLLISIONS";
             case OPT_VICII_SB_COLLISIONS:   return "SB_COLLISIONS";

             case OPT_DMA_DEBUG_ENABLE:      return "DMA_DEBUG_ENABLE";
             case OPT_DMA_DEBUG_MODE:        return "DMA_DEBUG_MODE";
             case OPT_DMA_DEBUG_OPACITY:     return "DMA_DEBUG_OPACITY";
             case OPT_DMA_DEBUG_CHANNEL0:    return "DMA_DEBUG_CHANNEL0";
             case OPT_DMA_DEBUG_CHANNEL1:    return "DMA_DEBUG_CHANNEL1";
             case OPT_DMA_DEBUG_CHANNEL2:    return "DMA_DEBUG_CHANNEL2";
             case OPT_DMA_DEBUG_CHANNEL3:    return "DMA_DEBUG_CHANNEL3";
             case OPT_DMA_DEBUG_CHANNEL4:    return "DMA_DEBUG_CHANNEL4";
             case OPT_DMA_DEBUG_CHANNEL5:    return "DMA_DEBUG_CHANNEL5";
             case OPT_DMA_DEBUG_COLOR0:      return "DMA_DEBUG_COLOR0";
             case OPT_DMA_DEBUG_COLOR1:      return "DMA_DEBUG_COLOR1";
             case OPT_DMA_DEBUG_COLOR2:      return "DMA_DEBUG_COLOR2";
             case OPT_DMA_DEBUG_COLOR3:      return "DMA_DEBUG_COLOR3";
             case OPT_DMA_DEBUG_COLOR4:      return "DMA_DEBUG_COLOR4";
             case OPT_DMA_DEBUG_COLOR5:      return "DMA_DEBUG_COLOR5";
             */

        case OPT_POWER_GRID:            return "Power Grid";

            /*
             case OPT_GLUE_LOGIC:            return "GLUE_LOGIC";

             case OPT_CIA_REVISION:          return "CIA_REVISION";
             case OPT_CIA_TIMER_B_BUG:       return "TIMER_B_BUG";

             case OPT_SID_ENABLE:            return "SID_ENABLE";
             case OPT_SID_ADDRESS:           return "SID_ADDRESS";
             case OPT_SID_REVISION:          return "SID_REVISION";
             case OPT_SID_FILTER:            return "SID_FILTER";
             case OPT_SID_POWER_SAVE:        return "SID_POWER_SAVE";
             case OPT_SID_ENGINE:            return "SID_ENGINE";
             case OPT_SID_SAMPLING:          return "SID_SAMPLING";
             case OPT_AUD_PAN:               return "AUDPAN";
             case OPT_AUD_VOL:               return "AUDVOL";
             case OPT_AUD_VOL_L:             return "AUDVOLL";
             case OPT_AUD_VOL_R:             return "AUDVOLR";

             case OPT_RAM_PATTERN:           return "RAM_PATTERN";
             case OPT_SAVE_ROMS:             return "SAVE_ROMS";

             case OPT_DRV_AUTO_CONFIG:       return "DRV_AUTO_CONFIG";
             case OPT_DRV_TYPE:              return "DRV_TYPE";
             case OPT_DRV_RAM:               return "OPT_DRV_RAM";
             case OPT_DRV_PARCABLE:          return "OPT_DRV_PARCABLE";
             case OPT_DRV_CONNECT:           return "DRV_CONNECT";
             case OPT_DRV_POWER_SWITCH:      return "DRV_POWER_SWITCH";
             case OPT_DRV_POWER_SAVE:        return "DRV_POWER_SAVE";
             case OPT_DRV_EJECT_DELAY:       return "DRV_EJECT_DELAY";
             case OPT_DRV_SWAP_DELAY:        return "DRV_SWAP_DELAY";
             case OPT_DRV_INSERT_DELAY:      return "DRV_INSERT_DELAY";
             case OPT_DRV_PAN:               return "DRV_PAN";
             case OPT_DRV_POWER_VOL:         return "DRV_POWER_VOL";
             case OPT_DRV_STEP_VOL:          return "DRV_STEP_VOL";
             case OPT_DRV_INSERT_VOL:        return "DRV_INSERT_VOL";
             case OPT_DRV_EJECT_VOL:         return "DRV_EJECT_VOL";

             case OPT_DAT_MODEL:             return "DAT_MODEL";
             case OPT_DAT_CONNECT:           return "DAT_CONNECT";

             case OPT_MOUSE_MODEL:           return "MOUSE_MODEL";
             case OPT_MOUSE_SHAKE_DETECT:    return "SHAKE_DETECTION";
             case OPT_MOUSE_VELOCITY:        return "MOUSE_VELOCITY";

             case OPT_JOY_AUTOFIRE:          return "AUTOFIRE";
             case OPT_JOY_AUTOFIRE_BULLETS:  return "AUTOFIRE_BULLETS";
             case OPT_JOY_AUTOFIRE_DELAY:    return "AUTOFIRE_DELAY";

             case OPT_COUNT:                 return "???";
             */
        default: return "<Missing help text>";

    }
    fatalError;
}

}
