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

#include "Aliases.h"
#include "Reflection.h"

#include "OptionTypes.h"

struct OptionEnum : util::Reflection<OptionEnum, Option> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = OPT_AUTOFIRE_DELAY;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return "OPT"; }
    static const char *key(Option value)
    {
        switch (value) {

            case OPT_HOST_REFRESH_RATE:     return "HOST.REFRESH_RATE";
            case OPT_HOST_SAMPLE_RATE:      return "HOST.SAMPLE_RATE";
            case OPT_HOST_FRAMEBUF_WIDTH:   return "HOST.FRAMEBUF_WIDTH";
            case OPT_HOST_FRAMEBUF_HEIGHT:  return "HOST.FRAMEBUF_HEIGHT";

            case OPT_EMU_WARP_BOOT:         return "EMU.WARP_BOOT";
            case OPT_EMU_WARP_MODE:         return "EMU.WARP_MODE";
            case OPT_EMU_VSYNC:             return "EMU.VSYNC";
            case OPT_EMU_TIME_LAPSE:        return "EMU.TIME_LAPSE";
            case OPT_EMU_RUN_AHEAD:         return "EMU.RUN_AHEAD";

            case OPT_VICII_REVISION:        return "VICII.REVISION";
            case OPT_VICII_PALETTE:         return "VICII.PALETTE";
            case OPT_VICII_BRIGHTNESS:      return "VICII.BRIGHTNESS";
            case OPT_VICII_CONTRAST:        return "VICII.CONTRAST";
            case OPT_VICII_SATURATION:      return "VICII.SATURATION";
            case OPT_VICII_GRAY_DOT_BUG:    return "VICII.GRAY_DOT_BUG";
            case OPT_VICII_POWER_SAVE:      return "VICII.POWER_SAVE";
            case OPT_VICII_HIDE_SPRITES:    return "VICII.HIDE_SPRITES";
            case OPT_VICII_CUT_LAYERS:      return "VICII.CUT_LAYERS";
            case OPT_VICII_CUT_OPACITY:     return "VICII.CUT_OPACITY";
            case OPT_VICII_SS_COLLISIONS:   return "VICII.SS_COLLISIONS";
            case OPT_VICII_SB_COLLISIONS:   return "VICII.SB_COLLISIONS";

            case OPT_DMA_DEBUG_ENABLE:      return "DMA_DEBUG.ENABLE";
            case OPT_DMA_DEBUG_MODE:        return "DMA_DEBUG.MODE";
            case OPT_DMA_DEBUG_OPACITY:     return "DMA_DEBUG.OPACITY";
            case OPT_DMA_DEBUG_CHANNEL0:    return "DMA_DEBUG.CHANNEL0";
            case OPT_DMA_DEBUG_CHANNEL1:    return "DMA_DEBUG.CHANNEL1";
            case OPT_DMA_DEBUG_CHANNEL2:    return "DMA_DEBUG.CHANNEL2";
            case OPT_DMA_DEBUG_CHANNEL3:    return "DMA_DEBUG.CHANNEL3";
            case OPT_DMA_DEBUG_CHANNEL4:    return "DMA_DEBUG.CHANNEL4";
            case OPT_DMA_DEBUG_CHANNEL5:    return "DMA_DEBUG.CHANNEL5";
            case OPT_DMA_DEBUG_COLOR0:      return "DMA_DEBUG.COLOR0";
            case OPT_DMA_DEBUG_COLOR1:      return "DMA_DEBUG.COLOR1";
            case OPT_DMA_DEBUG_COLOR2:      return "DMA_DEBUG.COLOR2";
            case OPT_DMA_DEBUG_COLOR3:      return "DMA_DEBUG.COLOR3";
            case OPT_DMA_DEBUG_COLOR4:      return "DMA_DEBUG.COLOR4";
            case OPT_DMA_DEBUG_COLOR5:      return "DMA_DEBUG.COLOR5";

            case OPT_POWER_GRID:            return "POWER_GRID";
            case OPT_GLUE_LOGIC:            return "GLUE_LOGIC";

            case OPT_CIA_REVISION:          return "CIA.REVISION";
            case OPT_CIA_TIMER_B_BUG:       return "CIA.TIMER_B_BUG";

            case OPT_SID_ENABLE:            return "SID.ENABLE";
            case OPT_SID_ADDRESS:           return "SID.ADDRESS";
            case OPT_SID_REVISION:          return "SID.REVISION";
            case OPT_SID_FILTER:            return "SID.FILTER";
            case OPT_SID_POWER_SAVE:        return "SID.POWER_SAVE";
            case OPT_SID_ENGINE:            return "SID.ENGINE";
            case OPT_SID_SAMPLING:          return "SID.SAMPLING";

            case OPT_AUD_PAN:               return "AUD.AUDPAN";
            case OPT_AUD_VOL:               return "AUD.VOL";
            case OPT_AUD_VOL_L:             return "AUD.VOLL";
            case OPT_AUD_VOL_R:             return "AUD.VOLR";

            case OPT_RAM_PATTERN:           return "RAM_PATTERN";
            case OPT_SAVE_ROMS:             return "SAVE_ROMS";

            case OPT_DRV_AUTO_CONFIG:       return "DRV.AUTO_CONFIG";
            case OPT_DRV_TYPE:              return "DRV.TYPE";
            case OPT_DRV_RAM:               return "DRV.RAM";
            case OPT_DRV_PARCABLE:          return "DRV.PARCABLE";
            case OPT_DRV_CONNECT:           return "DRV.CONNECT";
            case OPT_DRV_POWER_SWITCH:      return "DRV.POWER_SWITCH";
            case OPT_DRV_POWER_SAVE:        return "DRV.POWER_SAVE";
            case OPT_DRV_EJECT_DELAY:       return "DRV.EJECT_DELAY";
            case OPT_DRV_SWAP_DELAY:        return "DRV.SWAP_DELAY";
            case OPT_DRV_INSERT_DELAY:      return "DRV.INSERT_DELAY";
            case OPT_DRV_PAN:               return "DRV.PAN";
            case OPT_DRV_POWER_VOL:         return "DRV.POWER_VOL";
            case OPT_DRV_STEP_VOL:          return "DRV.STEP_VOL";
            case OPT_DRV_INSERT_VOL:        return "DRV.INSERT_VOL";
            case OPT_DRV_EJECT_VOL:         return "DRV.EJECT_VOL";

            case OPT_DAT_MODEL:             return "DAT.MODEL";
            case OPT_DAT_CONNECT:           return "DAT.CONNECT";

            case OPT_MOUSE_MODEL:           return "MOUSE.MODEL";
            case OPT_MOUSE_SHAKE_DETECT:    return "MOUSE.SHAKE_DETECTION";
            case OPT_MOUSE_VELOCITY:        return "MOUSE.VELOCITY";

            case OPT_AUTOFIRE:              return "AUTOFIRE.ENABLE";
            case OPT_AUTOFIRE_BURSTS:       return "AUTOFIRE.BURSTS";
            case OPT_AUTOFIRE_BULLETS:      return "AUTOFIRE.BULLETS";
            case OPT_AUTOFIRE_DELAY:        return "AUTOFIRE.DELAY";

            case OPT_REC_FRAME_RATE:        return "REC.FRAME_RATE";
            case OPT_REC_BIT_RATE:          return "REC.BIT_RATE";
            case OPT_REC_SAMPLE_RATE:       return "REC.SAMPLE_RATE";
            case OPT_REC_ASPECT_X:          return "REC.ASPECT_X";
            case OPT_REC_ASPECT_Y:          return "REC.ASPECT_Y";

            case OPT_COUNT:                 return "???";
        }
        return "???";
    }

    static const char *help(Option value)
    {
        switch (value) {

            case OPT_HOST_REFRESH_RATE:     return "Host refresh rate (fps)";
            case OPT_HOST_SAMPLE_RATE:      return "Host audio sample rate";
            case OPT_HOST_FRAMEBUF_WIDTH:   return "Window width";
            case OPT_HOST_FRAMEBUF_HEIGHT:  return "Window height";

            case OPT_EMU_WARP_BOOT:         return "Warp-boot duration";
            case OPT_EMU_WARP_MODE:         return "Warp activation";
            case OPT_EMU_VSYNC:             return "VSYNC mode";
            case OPT_EMU_TIME_LAPSE:        return "Speed adjust";
            case OPT_EMU_RUN_AHEAD:         return "Run-ahead frames";

            case OPT_VICII_REVISION:        return "Chip revision";
            case OPT_VICII_PALETTE:         return "Color palette";
            case OPT_VICII_BRIGHTNESS:      return "Monitor brightness";
            case OPT_VICII_CONTRAST:        return "Monitor contrast";
            case OPT_VICII_SATURATION:      return "Monitor saturation";
            case OPT_VICII_GRAY_DOT_BUG:    return "Emulate gray-dot bug";
            case OPT_VICII_POWER_SAVE:      return "Take fast paths";

            case OPT_VICII_HIDE_SPRITES:    return "Hide sprites";
            case OPT_VICII_CUT_LAYERS:      return "Cut out graphics layers";
            case OPT_VICII_CUT_OPACITY:     return "Opacity";
            case OPT_VICII_SS_COLLISIONS:   return "Sprite-sprite collisions";
            case OPT_VICII_SB_COLLISIONS:   return "Sprite-background collisions";

            case OPT_DMA_DEBUG_ENABLE:      return "DMA Debugger";
            case OPT_DMA_DEBUG_MODE:        return "DMA Debugger style";
            case OPT_DMA_DEBUG_OPACITY:     return "Opacity";
            case OPT_DMA_DEBUG_CHANNEL0:    return "Refresh cycles";
            case OPT_DMA_DEBUG_CHANNEL1:    return "Idle reads";
            case OPT_DMA_DEBUG_CHANNEL2:    return "Character accesses";
            case OPT_DMA_DEBUG_CHANNEL3:    return "Graphics accesses";
            case OPT_DMA_DEBUG_CHANNEL4:    return "Sprite-pointer accesses";
            case OPT_DMA_DEBUG_CHANNEL5:    return "Sprite-data accesses";
            case OPT_DMA_DEBUG_COLOR0:      return "Color of channel 0";
            case OPT_DMA_DEBUG_COLOR1:      return "Color of channel 1";
            case OPT_DMA_DEBUG_COLOR2:      return "Color of channel 2";
            case OPT_DMA_DEBUG_COLOR3:      return "Color of channel 3";
            case OPT_DMA_DEBUG_COLOR4:      return "Color of channel 4";
            case OPT_DMA_DEBUG_COLOR5:      return "Color of channel 5";

            case OPT_POWER_GRID:            return "Power grip stability";

            case OPT_GLUE_LOGIC:            return "Glue-logic type";

            case OPT_CIA_REVISION:          return "Chip revision";
            case OPT_CIA_TIMER_B_BUG:       return "Emulate Timer-B bug";

            case OPT_SID_ENABLE:            return "Enable";
            case OPT_SID_ADDRESS:           return "Memory location";
            case OPT_SID_REVISION:          return "Chip revision";
            case OPT_SID_FILTER:            return "Use filter";
            case OPT_SID_POWER_SAVE:        return "Take fast paths";
            case OPT_SID_ENGINE:            return "SID backend";
            case OPT_SID_SAMPLING:          return "Audio sampling rate";
            case OPT_AUD_PAN:               return "Pan";
            case OPT_AUD_VOL:               return "Channel volume";
            case OPT_AUD_VOL_L:             return "Master volume (left)";
            case OPT_AUD_VOL_R:             return "Master volume (right)";

            case OPT_RAM_PATTERN:           return "Memory start-up pattern";
            case OPT_SAVE_ROMS:             return "Save ROMs in snapshots";

            case OPT_DRV_AUTO_CONFIG:       return "Auto-configure drives";
            case OPT_DRV_TYPE:              return "Drive model";
            case OPT_DRV_RAM:               return "Drive RAM";
            case OPT_DRV_PARCABLE:          return "Parallel cable";
            case OPT_DRV_CONNECT:           return "Connected";
            case OPT_DRV_POWER_SWITCH:      return "Power switch";
            case OPT_DRV_POWER_SAVE:        return "Take fast paths";
            case OPT_DRV_EJECT_DELAY:       return "Disk eject delay";
            case OPT_DRV_SWAP_DELAY:        return "Disk swap delay";
            case OPT_DRV_INSERT_DELAY:      return "Disk insertion delay";
            case OPT_DRV_PAN:               return "Pan";
            case OPT_DRV_POWER_VOL:         return "Power switch volume";
            case OPT_DRV_STEP_VOL:          return "Head step volume";
            case OPT_DRV_INSERT_VOL:        return "Disk insertion volume";
            case OPT_DRV_EJECT_VOL:         return "Disk ejection volume";

            case OPT_DAT_MODEL:             return "Datasette model";
            case OPT_DAT_CONNECT:           return "Datasette connected";

            case OPT_MOUSE_MODEL:           return "Mouse model";
            case OPT_MOUSE_SHAKE_DETECT:    return "Detect a shaked mouse";
            case OPT_MOUSE_VELOCITY:        return "Mouse velocity";

            case OPT_AUTOFIRE:              return "Autofire";
            case OPT_AUTOFIRE_BURSTS:       return "Burst mode";
            case OPT_AUTOFIRE_BULLETS:      return "Number of bullets per burst";
            case OPT_AUTOFIRE_DELAY:        return "Autofire delay in frames";

            case OPT_REC_FRAME_RATE:        return "Frame rate of the recorded video";
            case OPT_REC_BIT_RATE:          return "Bit rate of the recorded video";
            case OPT_REC_SAMPLE_RATE:       return "Audio sample rate of the recorded video";
            case OPT_REC_ASPECT_X:          return "Numerator of the video's aspect ratio";
            case OPT_REC_ASPECT_Y:          return "Denumerator of the video's aspect ratio";

            case OPT_COUNT:                 return "???";
        }
        return "???";
    }
};
