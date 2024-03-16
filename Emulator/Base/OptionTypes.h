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

    // Screen recorder
    OPT_REC_FRAME_RATE,         ///< Frame rate of the recorded video
    OPT_REC_BIT_RATE,           ///< Bit rate of the recorded video
    OPT_REC_SAMPLE_RATE,        ///< Audio sample rate of the recorded video
    OPT_REC_ASPECT_X,           ///< Numerator of the video's aspect ratio
    OPT_REC_ASPECT_Y,           ///< Denumerator of the video's aspect ratio

    OPT_COUNT
};
typedef OPT Option;
