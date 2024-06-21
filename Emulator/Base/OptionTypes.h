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

#include "Reflection.h"

namespace vc64 {

//
// Enumerations
//

/// Configuration option
enum_long(OPT)
{
    // Host
    OPT_HOST_REFRESH_RATE,      ///< Refresh rate of the host display
    OPT_HOST_SAMPLE_RATE,       ///< Refresh rate of the host display
    OPT_HOST_FRAMEBUF_WIDTH,    ///< Current width of the emulator window
    OPT_HOST_FRAMEBUF_HEIGHT,   ///< Current height of the emulator window

    // Emulator
    OPT_EMU_WARP_BOOT,          ///< Warp-boot time in seconds
    OPT_EMU_WARP_MODE,          ///< Warp activation mode
    OPT_EMU_VSYNC,              ///< Derive the frame rate to the VSYNC signal
    OPT_EMU_SPEED_ADJUST,       ///< Speed adjustment in percent
    OPT_EMU_SNAPSHOTS,          ///< Automatically take a snapshots
    OPT_EMU_SNAPSHOT_DELAY,     ///< Delay between two snapshots in seconds
    OPT_EMU_RUN_AHEAD,          ///< Number of run-ahead frames

    // VICII
    OPT_VICII_REVISION,         ///< Chip revision
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

    // Video port
    OPT_VID_WHITE_NOISE,        ///< Generate white-noise when switched off
    ///<
    // Monitor
    OPT_MON_PALETTE,            ///< Color palette
    OPT_MON_BRIGHTNESS,         ///< Brightness
    OPT_MON_CONTRAST,           ///< Contrast
    OPT_MON_SATURATION,         ///< Color saturation
    OPT_MON_HCENTER,            ///< Horizontal centering
    OPT_MON_VCENTER,            ///< Vertical centering
    OPT_MON_HZOOM,              ///< Horizontal zoom
    OPT_MON_VZOOM,              ///< Vertical centering
    OPT_MON_UPSCALER,           ///< Pixel upscaler
    OPT_MON_BLUR,               ///< Blur enable
    OPT_MON_BLUR_RADIUS,        ///< Blur radius
    OPT_MON_BLOOM,              ///< Bloom enable
    OPT_MON_BLOOM_RADIUS,       ///< Bloom radius
    OPT_MON_BLOOM_BRIGHTNESS,   ///< Bloom brightness
    OPT_MON_BLOOM_WEIGHT,       ///< Bloom weight
    OPT_MON_DOTMASK,            ///< Dotmask pattern
    OPT_MON_DOTMASK_BRIGHTNESS, ///< Dotmask brightness
    OPT_MON_SCANLINES,          ///< Scanline mode
    OPT_MON_SCANLINE_BRIGHTNESS,///< Scanline brightness
    OPT_MON_SCANLINE_WEIGHT,    ///< Scanline weight
    OPT_MON_DISALIGNMENT,       ///< Cathode ray disalignment enable
    OPT_MON_DISALIGNMENT_H,     ///< Horizontal cathode ray disalignment
    OPT_MON_DISALIGNMENT_V,     ///< Vertical cathode ray disalignment

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
    OPT_SID_ENGINE,             ///< SID backend (e.g., reSID)
    OPT_SID_SAMPLING,           ///< Audio sampling mode
    OPT_SID_POWER_SAVE,         ///< Enable fast-paths

    // Audio port
    OPT_AUD_VOL0,               ///< Channel 0 volume
    OPT_AUD_VOL1,               ///< Channel 1 volume
    OPT_AUD_VOL2,               ///< Channel 2 volume
    OPT_AUD_VOL3,               ///< Channel 3 volume
    OPT_AUD_PAN0,               ///< Channel 0 panning
    OPT_AUD_PAN1,               ///< Channel 1 panning
    OPT_AUD_PAN2,               ///< Channel 2 panning
    OPT_AUD_PAN3,               ///< Channel 3 panning
    OPT_AUD_VOL_L,              ///< Master volume (left channel)
    OPT_AUD_VOL_R,              ///< Master volume (right channel)

    // Memory
    OPT_MEM_INIT_PATTERN,       ///< Ram initialization pattern
    OPT_MEM_HEATMAP,            ///< Memory heatmap
    OPT_MEM_SAVE_ROMS,          ///< Save Roms in snapshots

    // Drive
    OPT_DRV_AUTO_CONFIG,        ///< Auto-configure drives based on the Drive Rom
    OPT_DRV_TYPE,               ///< Drive model
    OPT_DRV_RAM,                ///< Drive Ram
    OPT_DRV_SAVE_ROMS,          ///< Save Roms in snapshots
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

    // Paddles
    OPT_PADDLE_ORIENTATION,     ///< Paddle value mapping scheme

    // Screen recorder
    OPT_REC_FRAME_RATE,         ///< Frame rate of the recorded video
    OPT_REC_BIT_RATE,           ///< Bit rate of the recorded video
    OPT_REC_SAMPLE_RATE,        ///< Audio sample rate of the recorded video
    OPT_REC_ASPECT_X,           ///< Numerator of the video's aspect ratio
    OPT_REC_ASPECT_Y,           ///< Denumerator of the video's aspect ratio

    OPT_COUNT
};
typedef OPT Option;

struct OptionEnum : util::Reflection<OptionEnum, Option> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = OPT_AUTOFIRE_DELAY;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return "OPT"; }
    static const char *key(long value)
    {
        switch (value) {

            case OPT_HOST_REFRESH_RATE:     return "HOST.REFRESH_RATE";
            case OPT_HOST_SAMPLE_RATE:      return "HOST.SAMPLE_RATE";
            case OPT_HOST_FRAMEBUF_WIDTH:   return "HOST.FRAMEBUF_WIDTH";
            case OPT_HOST_FRAMEBUF_HEIGHT:  return "HOST.FRAMEBUF_HEIGHT";

            case OPT_EMU_WARP_BOOT:         return "EMU.WARP_BOOT";
            case OPT_EMU_WARP_MODE:         return "EMU.WARP_MODE";
            case OPT_EMU_VSYNC:             return "EMU.VSYNC";
            case OPT_EMU_SPEED_ADJUST:      return "EMU.TIME_LAPSE";
            case OPT_EMU_SNAPSHOTS:         return "EMU.SNAPSHOTS";
            case OPT_EMU_SNAPSHOT_DELAY:    return "EMU.SNAPSHOT_DELAY";
            case OPT_EMU_RUN_AHEAD:         return "EMU.RUN_AHEAD";

            case OPT_VICII_REVISION:        return "VICII.REVISION";
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

            case OPT_VID_WHITE_NOISE:       return "VID.WHITE_NOISE";

            case OPT_MON_PALETTE:           return "MON.PALETTE";
            case OPT_MON_BRIGHTNESS:        return "MON.BRIGHTNESS";
            case OPT_MON_CONTRAST:          return "MON.CONTRAST";
            case OPT_MON_SATURATION:        return "MON.SATURATION";
            case OPT_MON_HCENTER:           return "MON.HCENTER";
            case OPT_MON_VCENTER:           return "MON.VCENTER";
            case OPT_MON_HZOOM:             return "MON.HZOOM";
            case OPT_MON_VZOOM:             return "MON.VZOOM";
            case OPT_MON_UPSCALER:          return "MON.UPSCALER";
            case OPT_MON_BLUR:              return "MON.BLUR";
            case OPT_MON_BLUR_RADIUS:       return "MON.BLUR_RADIUS";
            case OPT_MON_BLOOM:             return "MON.BLOOM";
            case OPT_MON_BLOOM_RADIUS:      return "MON.BLOOM_RADIUS";
            case OPT_MON_BLOOM_BRIGHTNESS:  return "MON.BLOOM_BRIGHTNESS";
            case OPT_MON_BLOOM_WEIGHT:      return "MON.BLOOM_WEIGHT";
            case OPT_MON_DOTMASK:           return "MON.DOTMASK";
            case OPT_MON_DOTMASK_BRIGHTNESS:    return "MON.DOTMASK_BRIGHTNESS";
            case OPT_MON_SCANLINES:             return "MON.SCANLINES";
            case OPT_MON_SCANLINE_BRIGHTNESS:   return "MON.SCANLINE_BRIGHTNESS";
            case OPT_MON_SCANLINE_WEIGHT:       return "MON.SCANLINE_WEIGHT";
            case OPT_MON_DISALIGNMENT:      return "MON.DISALIGNMENT";
            case OPT_MON_DISALIGNMENT_H:    return "MON.DISALIGNMENT_H";
            case OPT_MON_DISALIGNMENT_V:    return "MON.DISALIGNMENT_V";

            case OPT_POWER_GRID:            return "POWER_GRID";
            case OPT_GLUE_LOGIC:            return "GLUE_LOGIC";

            case OPT_CIA_REVISION:          return "CIA.REVISION";
            case OPT_CIA_TIMER_B_BUG:       return "CIA.TIMER_B_BUG";

            case OPT_SID_ENABLE:            return "SID.ENABLE";
            case OPT_SID_ADDRESS:           return "SID.ADDRESS";
            case OPT_SID_REVISION:          return "SID.REVISION";
            case OPT_SID_FILTER:            return "SID.FILTER";
            case OPT_SID_ENGINE:            return "SID.ENGINE";
            case OPT_SID_SAMPLING:          return "SID.SAMPLING";
            case OPT_SID_POWER_SAVE:        return "SID.POWER_SAVE";

            case OPT_AUD_VOL0:              return "AUD.VOL0";
            case OPT_AUD_VOL1:              return "AUD.VOL1";
            case OPT_AUD_VOL2:              return "AUD.VOL2";
            case OPT_AUD_VOL3:              return "AUD.VOL3";
            case OPT_AUD_PAN0:              return "AUD.PAN0";
            case OPT_AUD_PAN1:              return "AUD.PAN1";
            case OPT_AUD_PAN2:              return "AUD.PAN2";
            case OPT_AUD_PAN3:              return "AUD.PAN3";
            case OPT_AUD_VOL_L:             return "AUD.VOLL";
            case OPT_AUD_VOL_R:             return "AUD.VOLR";

            case OPT_MEM_INIT_PATTERN:      return "MEM.INIT_PATTERN";
            case OPT_MEM_HEATMAP:           return "MEM.HEATMAP";
            case OPT_MEM_SAVE_ROMS:         return "MEM.SAVE_ROMS";

            case OPT_DRV_AUTO_CONFIG:       return "DRV.AUTO_CONFIG";
            case OPT_DRV_TYPE:              return "DRV.TYPE";
            case OPT_DRV_RAM:               return "DRV.RAM";
            case OPT_DRV_SAVE_ROMS:         return "DRV.SAVE_ROMS";
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

            case OPT_PADDLE_ORIENTATION:    return "PADDLE_ORIENTATION";

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

            case OPT_HOST_REFRESH_RATE:     return "Host video refresh rate";
            case OPT_HOST_SAMPLE_RATE:      return "Host audio sample rate";
            case OPT_HOST_FRAMEBUF_WIDTH:   return "Window width";
            case OPT_HOST_FRAMEBUF_HEIGHT:  return "Window height";

            case OPT_EMU_WARP_BOOT:         return "Warp-boot duration";
            case OPT_EMU_WARP_MODE:         return "Warp activation";
            case OPT_EMU_VSYNC:             return "VSYNC mode";
            case OPT_EMU_SPEED_ADJUST:      return "Speed adjustment";
            case OPT_EMU_SNAPSHOTS:         return "Automatically take snapshots";
            case OPT_EMU_SNAPSHOT_DELAY:    return "Time span between two snapshots";
            case OPT_EMU_RUN_AHEAD:         return "Run-ahead frames";

            case OPT_VICII_REVISION:        return "Chip revision";
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

            case OPT_VID_WHITE_NOISE:       return "White noise";
                
            case OPT_MON_PALETTE:           return "Color palette";
            case OPT_MON_BRIGHTNESS:        return "Monitor brightness";
            case OPT_MON_CONTRAST:          return "Monitor contrast";
            case OPT_MON_SATURATION:        return "Monitor saturation";
            case OPT_MON_HCENTER:           return "Horizontal center";
            case OPT_MON_VCENTER:           return "Vertical center";
            case OPT_MON_HZOOM:             return "Horizontal zoom";
            case OPT_MON_VZOOM:             return "Vertical zoom";
            case OPT_MON_UPSCALER:          return "Upscaler";
            case OPT_MON_BLUR:              return "Blur";
            case OPT_MON_BLUR_RADIUS:       return "Blur radius";
            case OPT_MON_BLOOM:             return "Bloom";
            case OPT_MON_BLOOM_RADIUS:      return "Bloom radius";
            case OPT_MON_BLOOM_BRIGHTNESS:  return "Bloom brightness";
            case OPT_MON_BLOOM_WEIGHT:      return "Bloom weight";
            case OPT_MON_DOTMASK:           return "Dotmask";
            case OPT_MON_DOTMASK_BRIGHTNESS:    return "Dotmask brightness";
            case OPT_MON_SCANLINES:             return "Scanlines";
            case OPT_MON_SCANLINE_BRIGHTNESS:   return "Scanline brightness";
            case OPT_MON_SCANLINE_WEIGHT:       return "Scanline weight";
            case OPT_MON_DISALIGNMENT:      return "Cathode rays disalignment";
            case OPT_MON_DISALIGNMENT_H:    return "Horizontal cathode ray shift";
            case OPT_MON_DISALIGNMENT_V:    return "Vertical cathode ray shift";

            case OPT_POWER_GRID:            return "Power grid stability";
            case OPT_GLUE_LOGIC:            return "Glue-logic type";

            case OPT_CIA_REVISION:          return "Chip revision";
            case OPT_CIA_TIMER_B_BUG:       return "Emulate Timer-B bug";

            case OPT_SID_ENABLE:            return "Enable";
            case OPT_SID_ADDRESS:           return "Memory location";
            case OPT_SID_REVISION:          return "Chip revision";
            case OPT_SID_FILTER:            return "Use filter";
            case OPT_SID_ENGINE:            return "SID backend";
            case OPT_SID_SAMPLING:          return "Audio sampling rate";
            case OPT_SID_POWER_SAVE:        return "Take fast paths";
            case OPT_AUD_VOL0:              return "Channel 0 volume";
            case OPT_AUD_VOL1:              return "Channel 1 volume";
            case OPT_AUD_VOL2:              return "Channel 2 volume";
            case OPT_AUD_VOL3:              return "Channel 3 volume";
            case OPT_AUD_PAN0:              return "Channel 0 pan";
            case OPT_AUD_PAN1:              return "Channel 1 pan";
            case OPT_AUD_PAN2:              return "Channel 2 pan";
            case OPT_AUD_PAN3:              return "Channel 3 pan";
            case OPT_AUD_VOL_L:             return "Master volume (left)";
            case OPT_AUD_VOL_R:             return "Master volume (right)";

            case OPT_MEM_INIT_PATTERN:      return "Memory start-up pattern";
            case OPT_MEM_HEATMAP:           return "Heatmap";
            case OPT_MEM_SAVE_ROMS:         return "Save ROMs in snapshots";

            case OPT_DRV_AUTO_CONFIG:       return "Auto-configure drives";
            case OPT_DRV_TYPE:              return "Drive model";
            case OPT_DRV_RAM:               return "Drive RAM";
            case OPT_DRV_SAVE_ROMS:         return "Save Drive ROMs in snapshots";
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

            case OPT_PADDLE_ORIENTATION:    return "Paddle value mapping scheme";

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

}
