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

//
// Enumerations
//

/// Configuration option
enum_long(OPT)
{
    // Host
    OPT_HOST_REFRESH_RATE,       ///< Refresh rate of the host display
    OPT_HOST_SAMPLE_RATE,        ///< Refresh rate of the host display
    OPT_HOST_FRAMEBUF_WIDTH,     ///< Current width of the emulator window
    OPT_HOST_FRAMEBUF_HEIGHT,    ///< Current height of the emulator window

    // Emulator
    OPT_EMU_WARP_BOOT,          ///< Warp-boot time in seconds
    OPT_EMU_WARP_MODE,          ///< Warp activation mode
    OPT_EMU_VSYNC,              ///< Adapt the frame rate to the VSYNC signal
    OPT_EMU_TIME_LAPSE,         ///< Speed adjustment in percent
    OPT_EMU_RUN_AHEAD,          ///< Number of run-ahead frames

    // VICII
    OPT_VICII_REVISION,         ///< Chip revision
    OPT_VICII_PALETTE,          ///< Color palette
    OPT_VICII_BRIGHTNESS,       ///< Monitor brightness
    OPT_VICII_CONTRAST,         ///< Monitor contrast
    OPT_VICII_SATURATION,       ///< Monitor color saturation
    OPT_VICII_GRAY_DOT_BUG,     ///< Emulate gray-dot bug
    OPT_VICII_POWER_SAVE,       ///< Enable fast-paths
    OPT_VICII_HIDE_SPRITES,     ///< Hide some sprites
    OPT_VICII_CUT_LAYERS,       ///< Cutout some graphics layers
    OPT_VICII_CUT_OPACITY,      ///< Cutout opacity
    OPT_VICII_SS_COLLISIONS,    ///< Check for sprite-sprite collisions
    OPT_VICII_SB_COLLISIONS,    ///< Check for sprite-background collisions

    // DMA Debugger
    OPT_DMA_DEBUG_ENABLE,       ///< Global on/off switch for the DMA debugger
    OPT_DMA_DEBUG_MODE,         ///< DMA texture overlay mode
    OPT_DMA_DEBUG_OPACITY,      ///< DMA texture opacity
    OPT_DMA_DEBUG_CHANNEL0,     ///< Enable or disable channel 0
    OPT_DMA_DEBUG_CHANNEL1,     ///< Enable or disable channel 1
    OPT_DMA_DEBUG_CHANNEL2,     ///< Enable or disable channel 2
    OPT_DMA_DEBUG_CHANNEL3,     ///< Enable or disable channel 3
    OPT_DMA_DEBUG_CHANNEL4,     ///< Enable or disable channel 4
    OPT_DMA_DEBUG_CHANNEL5,     ///< Enable or disable channel 5
    OPT_DMA_DEBUG_COLOR0,       ///< Color for channel 0
    OPT_DMA_DEBUG_COLOR1,       ///< Color for channel 1
    OPT_DMA_DEBUG_COLOR2,       ///< Color for channel 2
    OPT_DMA_DEBUG_COLOR3,       ///< Color for channel 3
    OPT_DMA_DEBUG_COLOR4,       ///< Color for channel 4
    OPT_DMA_DEBUG_COLOR5,       ///< Color for channel 5

    // Power supply
    OPT_POWER_GRID,             ///< Power-grid stability (affects TOD)

    // Logic board
    OPT_GLUE_LOGIC,             ///< VICII glue-logic type

    // CIA
    OPT_CIA_REVISION,           ///< Chip revision
    OPT_CIA_TIMER_B_BUG,        ///< Emulate timer B bug

    // SID
    OPT_SID_ENABLE,             ///< Enable or disable SID
    OPT_SID_ADDRESS,            ///< Mapping address in memory
    OPT_SID_REVISION,           ///< Chip revision
    OPT_SID_FILTER,             ///< Enable or disables the audio filter
    OPT_SID_POWER_SAVE,         ///< Enable fast-paths
    OPT_SID_ENGINE,             ///< SID backend (e.g., reSID)
    OPT_SID_SAMPLING,           ///< Audio sampling mode

    // Audio backend
    OPT_AUD_PAN,                ///< Pan
    OPT_AUD_VOL,                ///< Master volume
    OPT_AUD_VOL_L,              ///< Left channel volume
    OPT_AUD_VOL_R,              ///< Right channel volume

    // Memory
    OPT_RAM_PATTERN,            ///< Ram initialization pattern
    OPT_SAVE_ROMS,              ///< Save Roms in snapshots

    // Drive
    OPT_DRV_AUTO_CONFIG,        ///< Auto-configure drives based on the Drive Rom
    OPT_DRV_TYPE,               ///< Drive model
    OPT_DRV_RAM,                ///< Drive Ram
    OPT_DRV_PARCABLE,           ///< Parallel cable type
    OPT_DRV_CONNECT,            ///< Connection status
    OPT_DRV_POWER_SWITCH,       ///< Power switch (on/off)
    OPT_DRV_POWER_SAVE,         ///< Enable fast-paths
    OPT_DRV_EJECT_DELAY,        ///< Disk ejection delay
    OPT_DRV_SWAP_DELAY,         ///< Disk swap delay
    OPT_DRV_INSERT_DELAY,       ///< Disk insertion delay
    OPT_DRV_PAN,                ///< Pan
    OPT_DRV_POWER_VOL,          ///< Volume (power-up sound)
    OPT_DRV_STEP_VOL,           ///< Volume (head steps)
    OPT_DRV_INSERT_VOL,         ///< Volume (disk insertion)
    OPT_DRV_EJECT_VOL,          ///< Volume (disk ejection)

    // Datasette
    OPT_DAT_MODEL,              ///< Datasette model
    OPT_DAT_CONNECT,            ///< Connection status

    // Mouse
    OPT_MOUSE_MODEL,            ///< Mouse model
    OPT_MOUSE_SHAKE_DETECT,     ///< Detect a shaking mouse
    OPT_MOUSE_VELOCITY,         ///< Mouse velocity

    // Joystick
    OPT_AUTOFIRE,               ///< Autofire status [on/off]
    OPT_AUTOFIRE_BURSTS,        ///< Burst mode (on/off)
    OPT_AUTOFIRE_BULLETS,       ///< Number of bullets per burst
    OPT_AUTOFIRE_DELAY,         ///< Delay between two shots [frames]

    OPT_COUNT
};
typedef OPT Option;

#ifdef __cplusplus
struct OptionEnum : util::Reflection<OptionEnum, Option> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = OPT_AUTOFIRE_DELAY;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return "OPT"; }
    static const char *key(Option value)
    {
        switch (value) {

            case OPT_HOST_REFRESH_RATE:     return "HOST_REFRESH_RATE";
            case OPT_HOST_SAMPLE_RATE:      return "HOST_SAMPLE_RATE";
            case OPT_HOST_FRAMEBUF_WIDTH:   return "HOST_FRAMEBUF_WIDTH";
            case OPT_HOST_FRAMEBUF_HEIGHT:  return "HOST_FRAMEBUF_HEIGHT";

            case OPT_EMU_WARP_BOOT:         return "WARP_BOOT";
            case OPT_EMU_WARP_MODE:         return "WARP_MODE";
            case OPT_EMU_VSYNC:             return "VSYNC";
            case OPT_EMU_TIME_LAPSE:        return "TIME_LAPSE";
            case OPT_EMU_RUN_AHEAD:         return "RUN_AHEAD";

            case OPT_VICII_REVISION:        return "VICII.REVISION";
            case OPT_VICII_PALETTE:         return "PALETTE";
            case OPT_VICII_BRIGHTNESS:      return "BRIGHTNESS";
            case OPT_VICII_CONTRAST:        return "CONTRAST";
            case OPT_VICII_SATURATION:      return "SATURATION";
            case OPT_VICII_GRAY_DOT_BUG:    return "GRAY_DOT_BUG";
            case OPT_VICII_POWER_SAVE:      return "VICII.POWER_SAVE";

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

            case OPT_POWER_GRID:            return "POWER_GRID";

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

            case OPT_AUTOFIRE:              return "AUTOFIRE";
            case OPT_AUTOFIRE_BURSTS:       return "AUTOFIRE_BURSTS";
            case OPT_AUTOFIRE_BULLETS:      return "AUTOFIRE_BULLETS";
            case OPT_AUTOFIRE_DELAY:        return "AUTOFIRE_DELAY";

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
            case OPT_DAT_CONNECT:           return "Connected";

            case OPT_MOUSE_MODEL:           return "Mouse model";
            case OPT_MOUSE_SHAKE_DETECT:    return "Detect a shaked mouse";
            case OPT_MOUSE_VELOCITY:        return "Mouse velocity";

            case OPT_AUTOFIRE:              return "Autofire";
            case OPT_AUTOFIRE_BURSTS:       return "Burst mode";
            case OPT_AUTOFIRE_BULLETS:      return "Number of bullets per burst";
            case OPT_AUTOFIRE_DELAY:        return "Autofire delay in frames";

            case OPT_COUNT:                 return "???";
        }
        return "???";
    }
};
#endif
