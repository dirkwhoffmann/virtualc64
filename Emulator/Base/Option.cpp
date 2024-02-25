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

std::unique_ptr<OptionParser>
OptionParser::create(Option opt)
{
    auto enumParser = [&]<typename T>() { return std::unique_ptr<EnumParser<T>>(new EnumParser<T>(opt)); };
    auto boolParser = [&]() { return std::unique_ptr<BoolParser>(new BoolParser(opt)); };
    auto numParser  = [&]() { return std::unique_ptr<NumParser>(new NumParser(opt)); };

    switch (opt) {

        case OPT_EMU_WARP_MODE:             return enumParser.template operator()<VICIIRevisionEnum>();
        case OPT_EMU_WARP_BOOT:             return boolParser();
        case OPT_EMU_VSYNC:                 return boolParser();
        case OPT_EMU_TIME_LAPSE:            return numParser();
        case OPT_EMU_RUN_AHEAD:             return numParser();

        case OPT_HOST_SAMPLE_RATE:          return numParser();

        case OPT_HOST_REFRESH_RATE:         return numParser();
        case OPT_HOST_FRAMEBUF_WIDTH:       return numParser();
        case OPT_HOST_FRAMEBUF_HEIGHT:      return numParser();

        case OPT_VICII_REVISION:            return enumParser.template operator()<VICIIRevisionEnum>();
        case OPT_VICII_PALETTE:             return enumParser.template operator()<PaletteEnum>();
        case OPT_VICII_BRIGHTNESS:          return numParser();
        case OPT_VICII_CONTRAST:            return numParser();
        case OPT_VICII_SATURATION:          return numParser();
        case OPT_VICII_GRAY_DOT_BUG:        return boolParser();
        case OPT_VICII_POWER_SAVE:          return boolParser();
        case OPT_VICII_HIDE_SPRITES:        return boolParser();
        case OPT_VICII_SS_COLLISIONS:       return boolParser();
        case OPT_VICII_SB_COLLISIONS:       return boolParser();
        case OPT_GLUE_LOGIC:                return enumParser.template operator()<GlueLogicEnum>();

        case OPT_VICII_CUT_LAYERS:          return numParser();
        case OPT_VICII_CUT_OPACITY:         return numParser();
        case OPT_DMA_DEBUG_ENABLE:          return boolParser();
        case OPT_DMA_DEBUG_MODE:            return enumParser.template operator()<DmaDisplayModeEnum>();
        case OPT_DMA_DEBUG_OPACITY:         return numParser();
        case OPT_DMA_DEBUG_CHANNEL0:        return boolParser();
        case OPT_DMA_DEBUG_CHANNEL1:        return boolParser();
        case OPT_DMA_DEBUG_CHANNEL2:        return boolParser();
        case OPT_DMA_DEBUG_CHANNEL3:        return boolParser();
        case OPT_DMA_DEBUG_CHANNEL4:        return boolParser();
        case OPT_DMA_DEBUG_CHANNEL5:        return boolParser();
        case OPT_DMA_DEBUG_COLOR0:          return numParser();
        case OPT_DMA_DEBUG_COLOR1:          return numParser();
        case OPT_DMA_DEBUG_COLOR2:          return numParser();
        case OPT_DMA_DEBUG_COLOR3:          return numParser();
        case OPT_DMA_DEBUG_COLOR4:          return numParser();
        case OPT_DMA_DEBUG_COLOR5:          return numParser();

        case OPT_POWER_GRID:                return enumParser.template operator()<PowerGridEnum>();

        case OPT_CIA_REVISION:              return enumParser.template operator()<CIARevisionEnum>();
        case OPT_CIA_TIMER_B_BUG:           return boolParser();

        case OPT_SID_ENABLE:                return numParser();
        case OPT_SID_ADDRESS:               return numParser();

        case OPT_SID_REVISION:              return enumParser.template operator()<SIDRevisionEnum>();
        case OPT_SID_FILTER:                return boolParser();
        case OPT_SID_SAMPLING:              return enumParser.template operator()<SamplingMethodEnum>();
        case OPT_SID_POWER_SAVE:            return boolParser();
        case OPT_SID_ENGINE:                return enumParser.template operator()<SIDEngineEnum>();
        case OPT_AUD_PAN:                   return numParser();
        case OPT_AUD_VOL:                   return numParser();
        case OPT_AUD_VOL_L:                 return numParser();
        case OPT_AUD_VOL_R:                 return numParser();

        case OPT_RAM_PATTERN:               return enumParser.template operator()<RamPatternEnum>();

        case OPT_SAVE_ROMS:                 return boolParser();

        case OPT_DRV_AUTO_CONFIG:           return boolParser();
        case OPT_DRV_TYPE:                  return enumParser.template operator()<DriveTypeEnum>();
        case OPT_DRV_RAM:                   return enumParser.template operator()<DriveRamEnum>();
        case OPT_DRV_PARCABLE:              return enumParser.template operator()<ParCableTypeEnum>();
        case OPT_DRV_CONNECT:               return boolParser();
        case OPT_DRV_POWER_SWITCH:          return boolParser();
        case OPT_DRV_POWER_SAVE:            return boolParser();
        case OPT_DRV_EJECT_DELAY:           return numParser();
        case OPT_DRV_SWAP_DELAY:            return numParser();
        case OPT_DRV_INSERT_DELAY:          return numParser();
        case OPT_DRV_PAN:                   return numParser();
        case OPT_DRV_POWER_VOL:             return numParser();
        case OPT_DRV_STEP_VOL:              return numParser();
        case OPT_DRV_INSERT_VOL:            return numParser();
        case OPT_DRV_EJECT_VOL:             return numParser();

        case OPT_DAT_MODEL:                 return enumParser.template operator()<DatasetteModelEnum>();
        case OPT_DAT_CONNECT:               return boolParser();

        case OPT_MOUSE_MODEL:               return enumParser.template operator()<MouseModelEnum>();
        case OPT_MOUSE_SHAKE_DETECT:        return boolParser();
        case OPT_MOUSE_VELOCITY:            return numParser();

        case OPT_JOY_AUTOFIRE:              return boolParser();
        case OPT_JOY_AUTOFIRE_BULLETS:      return numParser();
        case OPT_JOY_AUTOFIRE_DELAY:        return numParser();

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
