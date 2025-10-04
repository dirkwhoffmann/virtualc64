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
enum class Opt : long
{
    // Host
    HOST_REFRESH_RATE,      ///< Video refresh rate of the host display
    HOST_SAMPLE_RATE,       ///< Audio sample rate of the host display
    HOST_FRAMEBUF_WIDTH,    ///< Current width of the emulator window
    HOST_FRAMEBUF_HEIGHT,   ///< Current height of the emulator window

    // C64
    C64_WARP_BOOT,          ///< Warp-boot time in seconds
    C64_WARP_MODE,          ///< Warp activation mode
    C64_VSYNC,              ///< Derive the frame rate to the VSYNC signal
    C64_SPEED_BOOST,        ///< Speed adjustment in percent
    C64_RUN_AHEAD,          ///< Number of run-ahead frames

    // Snapshots
    C64_SNAP_AUTO,          ///< Automatically take a snapshots
    C64_SNAP_DELAY,         ///< Delay between two snapshots in seconds
    C64_SNAP_COMPRESSOR,    ///< Snapshot compression method

    // CPU
    DASM_NUMBERS,           ///< Disassembler number format
    
    // VICII
    VICII_REVISION,         ///< Chip revision
    VICII_GRAY_DOT_BUG,     ///< Emulate gray-dot bug
    VICII_POWER_SAVE,       ///< Enable fast-paths
    VICII_HIDE_SPRITES,     ///< Hide some sprites
    VICII_CUT_LAYERS,       ///< Cutout some graphics layers
    VICII_CUT_OPACITY,      ///< Cutout opacity
    VICII_SS_COLLISIONS,    ///< Check for sprite-sprite collisions
    VICII_SB_COLLISIONS,    ///< Check for sprite-background collisions

    // DMA Debugger
    DMA_DEBUG_ENABLE,       ///< Global on/off switch for the DMA debugger
    DMA_DEBUG_OVERLAY,      ///< Overlay DMA texture onto emulator texture
    DMA_DEBUG_MODE,         ///< DMA texture overlay mode
    DMA_DEBUG_OPACITY,      ///< DMA texture opacity
    DMA_DEBUG_CHANNEL0,     ///< Enable or disable channel 0
    DMA_DEBUG_CHANNEL1,     ///< Enable or disable channel 1
    DMA_DEBUG_CHANNEL2,     ///< Enable or disable channel 2
    DMA_DEBUG_CHANNEL3,     ///< Enable or disable channel 3
    DMA_DEBUG_CHANNEL4,     ///< Enable or disable channel 4
    DMA_DEBUG_CHANNEL5,     ///< Enable or disable channel 5
    DMA_DEBUG_COLOR0,       ///< Color for channel 0
    DMA_DEBUG_COLOR1,       ///< Color for channel 1
    DMA_DEBUG_COLOR2,       ///< Color for channel 2
    DMA_DEBUG_COLOR3,       ///< Color for channel 3
    DMA_DEBUG_COLOR4,       ///< Color for channel 4
    DMA_DEBUG_COLOR5,       ///< Color for channel 5

    // Expansion port
    EXP_REU_SPEED,          ///< Transfer speed of the RAM Extension Unit

    // User port
    USR_DEVICE,             ///< Device connected to the user port

    // Video port
    VID_WHITE_NOISE,        ///< Generate white-noise when switched off

    // Monitor
    MON_PALETTE,            ///< Color palette
    MON_BRIGHTNESS,         ///< Brightness
    MON_CONTRAST,           ///< Contrast
    MON_SATURATION,         ///< Color saturation
    MON_HCENTER,            ///< Horizontal centering
    MON_VCENTER,            ///< Vertical centering
    MON_HZOOM,              ///< Horizontal zoom
    MON_VZOOM,              ///< Vertical centering
    MON_UPSCALER,           ///< Pixel upscaler
    MON_BLUR,               ///< Blur enable
    MON_BLUR_RADIUS,        ///< Blur radius
    MON_BLOOM,              ///< Bloom enable
    MON_BLOOM_RADIUS,       ///< Bloom radius
    MON_BLOOM_BRIGHTNESS,   ///< Bloom brightness
    MON_BLOOM_WEIGHT,       ///< Bloom weight
    MON_DOTMASK,            ///< Dotmask pattern
    MON_DOTMASK_BRIGHTNESS, ///< Dotmask brightness
    MON_SCANLINES,          ///< Scanline mode
    MON_SCANLINE_BRIGHTNESS,///< Scanline brightness
    MON_SCANLINE_WEIGHT,    ///< Scanline weight
    MON_DISALIGNMENT,       ///< Cathode ray disalignment enable
    MON_DISALIGNMENT_H,     ///< Horizontal cathode ray disalignment
    MON_DISALIGNMENT_V,     ///< Vertical cathode ray disalignment

    // Power supply
    POWER_GRID,             ///< Power-grid stability (affects TOD)

    // Logic board
    GLUE_LOGIC,             ///< VICII glue-logic type

    // CIA
    CIA_REVISION,           ///< Chip revision
    CIA_TIMER_B_BUG,        ///< Emulate timer B bug
    CIA_IDLE_SLEEP,         ///< Enter idle state while not in use

    // SID
    SID_ENABLE,             ///< Enable or disable SID
    SID_ADDRESS,            ///< Mapping address in memory
    SID_REV,           ///< Chip revision
    SID_FILTER,             ///< Enable or disables the audio filter
    SID_ENGINE,             ///< SID backend (e.g., reSID)
    SID_SAMPLING,           ///< Audio sampling mode
    SID_POWER_SAVE,         ///< Enable fast-paths

    // Audio port
    AUD_VOL0,               ///< Channel 0 volume
    AUD_VOL1,               ///< Channel 1 volume
    AUD_VOL2,               ///< Channel 2 volume
    AUD_VOL3,               ///< Channel 3 volume
    AUD_PAN0,               ///< Channel 0 panning
    AUD_PAN1,               ///< Channel 1 panning
    AUD_PAN2,               ///< Channel 2 panning
    AUD_PAN3,               ///< Channel 3 panning
    AUD_VOL_L,              ///< Master volume (left channel)
    AUD_VOL_R,              ///< Master volume (right channel)

    // Memory
    MEM_INIT_PATTERN,       ///< Ram initialization pattern
    MEM_HEATMAP,            ///< Memory heatmap
    MEM_SAVE_ROMS,          ///< Save Roms in snapshots

    // Drive
    DRV_AUTO_CONFIG,        ///< Auto-configure drives based on the Drive Rom
    DRV_TYPE,               ///< Drive model
    DRV_RAM,                ///< Drive Ram
    DRV_SAVE_ROMS,          ///< Save Roms in snapshots
    DRV_PARCABLE,           ///< Parallel cable type
    DRV_CONNECT,            ///< Connection status
    DRV_POWER_SWITCH,       ///< Power switch (on/off)
    DRV_POWER_SAVE,         ///< Enable fast-paths
    DRV_EJECT_DELAY,        ///< Disk ejection delay
    DRV_SWAP_DELAY,         ///< Disk swap delay
    DRV_INSERT_DELAY,       ///< Disk insertion delay
    DRV_PAN,                ///< Pan
    DRV_POWER_VOL,          ///< Volume (power-up sound)
    DRV_STEP_VOL,           ///< Volume (head steps)
    DRV_INSERT_VOL,         ///< Volume (disk insertion)
    DRV_EJECT_VOL,          ///< Volume (disk ejection)

    // Datasette
    DAT_MODEL,              ///< Datasette model
    DAT_CONNECT,            ///< Connection status

    // Mouse
    MOUSE_MODEL,            ///< Mouse model
    MOUSE_SHAKE_DETECT,     ///< Detect a shaking mouse
    MOUSE_VELOCITY,         ///< Mouse velocity

    // Joystick
    AUTOFIRE,               ///< Autofire status [on/off]
    AUTOFIRE_BURSTS,        ///< Burst mode (on/off)
    AUTOFIRE_BULLETS,       ///< Number of bullets per burst
    AUTOFIRE_DELAY,         ///< Delay between two shots [frames]

    // Paddles
    PADDLE_ORIENTATION,     ///< Paddle value mapping scheme

    // RS232 adapter
    RS232_DEVICE,           ///< Device connected to the RS232 adapter
    RS232_BAUD,             ///< Transmission rate

    // Screen recorder
    REC_FRAME_RATE,         ///< Frame rate of the recorded video
    REC_BIT_RATE,           ///< Bit rate of the recorded video
    REC_SAMPLE_RATE,        ///< Audio sample rate of the recorded video
    REC_ASPECT_X,           ///< Numerator of the video's aspect ratio
    REC_ASPECT_Y,           ///< Denumerator of the video's aspect ratio

    // Remote servers
    SRV_PORT,
    SRV_PROTOCOL,
    SRV_AUTORUN,
    SRV_VERBOSE,

    // Regression tester
    DBG_DEBUGCART,          ///< Emulate the VICE debug cartridge
    DBG_WATCHDOG,           ///< Watchdog delay in frames

    COUNT
};

struct OptEnum : Reflection<OptEnum, Opt> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = long(Opt::DBG_WATCHDOG);

    static const char *_key(Opt value)
    {
        switch (value) {

            case Opt::HOST_REFRESH_RATE:     return "HOST.REFRESH_RATE";
            case Opt::HOST_SAMPLE_RATE:      return "HOST.SAMPLE_RATE";
            case Opt::HOST_FRAMEBUF_WIDTH:   return "HOST.FRAMEBUF_WIDTH";
            case Opt::HOST_FRAMEBUF_HEIGHT:  return "HOST.FRAMEBUF_HEIGHT";

            case Opt::C64_WARP_BOOT:         return "C64.WARP_BOOT";
            case Opt::C64_WARP_MODE:         return "C64.WARP_MODE";
            case Opt::C64_VSYNC:             return "C64.VSYNC";
            case Opt::C64_SPEED_BOOST:       return "C64.SPEED_BOOST";
            case Opt::C64_RUN_AHEAD:         return "C64.RUN_AHEAD";
            
            case Opt::C64_SNAP_AUTO:         return "C64.SNAP_AUTO";
            case Opt::C64_SNAP_DELAY:        return "C64.SNAP_DELAY";
            case Opt::C64_SNAP_COMPRESSOR:   return "C64.SNAP_COMPRESSOR";

            case Opt::DASM_NUMBERS:          return "CPU.DASM_NUMBERS";
                
            case Opt::VICII_REVISION:        return "VICII.REVISION";
            case Opt::VICII_GRAY_DOT_BUG:    return "VICII.GRAY_DOT_BUG";
            case Opt::VICII_POWER_SAVE:      return "VICII.POWER_SAVE";
            case Opt::VICII_HIDE_SPRITES:    return "VICII.HIDE_SPRITES";
            case Opt::VICII_CUT_LAYERS:      return "VICII.CUT_LAYERS";
            case Opt::VICII_CUT_OPACITY:     return "VICII.CUT_OPACITY";
            case Opt::VICII_SS_COLLISIONS:   return "VICII.SS_COLLISIONS";
            case Opt::VICII_SB_COLLISIONS:   return "VICII.SB_COLLISIONS";

            case Opt::DMA_DEBUG_ENABLE:      return "DMA_DEBUG.ENABLE";
            case Opt::DMA_DEBUG_OVERLAY:     return "DMA_DEBUG.OVERLAY";
            case Opt::DMA_DEBUG_MODE:        return "DMA_DEBUG.MODE";
            case Opt::DMA_DEBUG_OPACITY:     return "DMA_DEBUG.OPACITY";
            case Opt::DMA_DEBUG_CHANNEL0:    return "DMA_DEBUG.CHANNEL0";
            case Opt::DMA_DEBUG_CHANNEL1:    return "DMA_DEBUG.CHANNEL1";
            case Opt::DMA_DEBUG_CHANNEL2:    return "DMA_DEBUG.CHANNEL2";
            case Opt::DMA_DEBUG_CHANNEL3:    return "DMA_DEBUG.CHANNEL3";
            case Opt::DMA_DEBUG_CHANNEL4:    return "DMA_DEBUG.CHANNEL4";
            case Opt::DMA_DEBUG_CHANNEL5:    return "DMA_DEBUG.CHANNEL5";
            case Opt::DMA_DEBUG_COLOR0:      return "DMA_DEBUG.COLOR0";
            case Opt::DMA_DEBUG_COLOR1:      return "DMA_DEBUG.COLOR1";
            case Opt::DMA_DEBUG_COLOR2:      return "DMA_DEBUG.COLOR2";
            case Opt::DMA_DEBUG_COLOR3:      return "DMA_DEBUG.COLOR3";
            case Opt::DMA_DEBUG_COLOR4:      return "DMA_DEBUG.COLOR4";
            case Opt::DMA_DEBUG_COLOR5:      return "DMA_DEBUG.COLOR5";

            case Opt::EXP_REU_SPEED:         return "EXP.REU_SPEED";

            case Opt::USR_DEVICE:            return "USR.DEVICE";

            case Opt::VID_WHITE_NOISE:       return "VID.WHITE_NOISE";

            case Opt::MON_PALETTE:           return "MON.PALETTE";
            case Opt::MON_BRIGHTNESS:        return "MON.BRIGHTNESS";
            case Opt::MON_CONTRAST:          return "MON.CONTRAST";
            case Opt::MON_SATURATION:        return "MON.SATURATION";
            case Opt::MON_HCENTER:           return "MON.HCENTER";
            case Opt::MON_VCENTER:           return "MON.VCENTER";
            case Opt::MON_HZOOM:             return "MON.HZOOM";
            case Opt::MON_VZOOM:             return "MON.VZOOM";
            case Opt::MON_UPSCALER:          return "MON.UPSCALER";
            case Opt::MON_BLUR:              return "MON.BLUR";
            case Opt::MON_BLUR_RADIUS:       return "MON.BLUR_RADIUS";
            case Opt::MON_BLOOM:             return "MON.BLOOM";
            case Opt::MON_BLOOM_RADIUS:      return "MON.BLOOM_RADIUS";
            case Opt::MON_BLOOM_BRIGHTNESS:  return "MON.BLOOM_BRIGHTNESS";
            case Opt::MON_BLOOM_WEIGHT:      return "MON.BLOOM_WEIGHT";
            case Opt::MON_DOTMASK:           return "MON.DOTMASK";
            case Opt::MON_DOTMASK_BRIGHTNESS:    return "MON.DOTMASK_BRIGHTNESS";
            case Opt::MON_SCANLINES:             return "MON.SCANLINES";
            case Opt::MON_SCANLINE_BRIGHTNESS:   return "MON.SCANLINE_BRIGHTNESS";
            case Opt::MON_SCANLINE_WEIGHT:       return "MON.SCANLINE_WEIGHT";
            case Opt::MON_DISALIGNMENT:      return "MON.DISALIGNMENT";
            case Opt::MON_DISALIGNMENT_H:    return "MON.DISALIGNMENT_H";
            case Opt::MON_DISALIGNMENT_V:    return "MON.DISALIGNMENT_V";

            case Opt::POWER_GRID:            return "POWER_GRID";
            case Opt::GLUE_LOGIC:            return "GLUE_LOGIC";

            case Opt::CIA_REVISION:          return "CIA.REVISION";
            case Opt::CIA_TIMER_B_BUG:       return "CIA.TIMER_B_BUG";
            case Opt::CIA_IDLE_SLEEP:        return "CIA.IDLE_SLEEP";

            case Opt::SID_ENABLE:            return "SID.ENABLE";
            case Opt::SID_ADDRESS:           return "SID.ADDRESS";
            case Opt::SID_REV:          return "SID.REVISION";
            case Opt::SID_FILTER:            return "SID.FILTER";
            case Opt::SID_ENGINE:            return "SID.ENGINE";
            case Opt::SID_SAMPLING:          return "SID.SAMPLING";
            case Opt::SID_POWER_SAVE:        return "SID.POWER_SAVE";

            case Opt::AUD_VOL0:              return "AUD.VOL0";
            case Opt::AUD_VOL1:              return "AUD.VOL1";
            case Opt::AUD_VOL2:              return "AUD.VOL2";
            case Opt::AUD_VOL3:              return "AUD.VOL3";
            case Opt::AUD_PAN0:              return "AUD.PAN0";
            case Opt::AUD_PAN1:              return "AUD.PAN1";
            case Opt::AUD_PAN2:              return "AUD.PAN2";
            case Opt::AUD_PAN3:              return "AUD.PAN3";
            case Opt::AUD_VOL_L:             return "AUD.VOLL";
            case Opt::AUD_VOL_R:             return "AUD.VOLR";

            case Opt::MEM_INIT_PATTERN:      return "MEM.INIT_PATTERN";
            case Opt::MEM_HEATMAP:           return "MEM.HEATMAP";
            case Opt::MEM_SAVE_ROMS:         return "MEM.SAVE_ROMS";

            case Opt::DRV_AUTO_CONFIG:       return "DRV.AUTO_CONFIG";
            case Opt::DRV_TYPE:              return "DRV.TYPE";
            case Opt::DRV_RAM:               return "DRV.RAM";
            case Opt::DRV_SAVE_ROMS:         return "DRV.SAVE_ROMS";
            case Opt::DRV_PARCABLE:          return "DRV.PARCABLE";
            case Opt::DRV_CONNECT:           return "DRV.CONNECT";
            case Opt::DRV_POWER_SWITCH:      return "DRV.POWER_SWITCH";
            case Opt::DRV_POWER_SAVE:        return "DRV.POWER_SAVE";
            case Opt::DRV_EJECT_DELAY:       return "DRV.EJECT_DELAY";
            case Opt::DRV_SWAP_DELAY:        return "DRV.SWAP_DELAY";
            case Opt::DRV_INSERT_DELAY:      return "DRV.INSERT_DELAY";
            case Opt::DRV_PAN:               return "DRV.PAN";
            case Opt::DRV_POWER_VOL:         return "DRV.POWER_VOL";
            case Opt::DRV_STEP_VOL:          return "DRV.STEP_VOL";
            case Opt::DRV_INSERT_VOL:        return "DRV.INSERT_VOL";
            case Opt::DRV_EJECT_VOL:         return "DRV.EJECT_VOL";

            case Opt::DAT_MODEL:             return "DAT.MODEL";
            case Opt::DAT_CONNECT:           return "DAT.CONNECT";

            case Opt::MOUSE_MODEL:           return "MOUSE.MODEL";
            case Opt::MOUSE_SHAKE_DETECT:    return "MOUSE.SHAKE_DETECTION";
            case Opt::MOUSE_VELOCITY:        return "MOUSE.VELOCITY";

            case Opt::AUTOFIRE:              return "AUTOFIRE.ENABLE";
            case Opt::AUTOFIRE_BURSTS:       return "AUTOFIRE.BURSTS";
            case Opt::AUTOFIRE_BULLETS:      return "AUTOFIRE.BULLETS";
            case Opt::AUTOFIRE_DELAY:        return "AUTOFIRE.DELAY";

            case Opt::PADDLE_ORIENTATION:    return "PADDLE.ORIENTATION";

            case Opt::RS232_DEVICE:          return "RS232.DEVICE";
            case Opt::RS232_BAUD:            return "RS232.BAUD";

            case Opt::REC_FRAME_RATE:        return "REC.FRAME_RATE";
            case Opt::REC_BIT_RATE:          return "REC.BIT_RATE";
            case Opt::REC_SAMPLE_RATE:       return "REC.SAMPLE_RATE";
            case Opt::REC_ASPECT_X:          return "REC.ASPECT_X";
            case Opt::REC_ASPECT_Y:          return "REC.ASPECT_Y";

            case Opt::SRV_PORT:              return "SRV.PORT";
            case Opt::SRV_PROTOCOL:          return "SRV.PROTOCOL";
            case Opt::SRV_AUTORUN:           return "SRV.AUTORUN";
            case Opt::SRV_VERBOSE:           return "SRV.VERBOSE";

            case Opt::DBG_DEBUGCART:         return "DBG.DEBUGCART";
            case Opt::DBG_WATCHDOG:          return "DBG.WATCHDOG";

            case Opt::COUNT:                 return "???";
        }
        return "???";
    }

    static const char *help(Opt value)
    {
        switch (value) {

            case Opt::HOST_REFRESH_RATE:     return "Host video refresh rate";
            case Opt::HOST_SAMPLE_RATE:      return "Host audio sample rate";
            case Opt::HOST_FRAMEBUF_WIDTH:   return "Window width";
            case Opt::HOST_FRAMEBUF_HEIGHT:  return "Window height";

            case Opt::C64_WARP_BOOT:         return "Warp-boot duration";
            case Opt::C64_WARP_MODE:         return "Warp activation";
            case Opt::C64_VSYNC:             return "VSYNC mode";
            case Opt::C64_SPEED_BOOST:      return "Speed adjustment";
            case Opt::C64_RUN_AHEAD:         return "Run-ahead frames";

            case Opt::C64_SNAP_AUTO:         return "Automatically take snapshots";
            case Opt::C64_SNAP_DELAY:        return "Time span between two snapshots";
            case Opt::C64_SNAP_COMPRESSOR:   return "Snapshot compression method";

            case Opt::DASM_NUMBERS:          return "Disassembler number format";
                
            case Opt::VICII_REVISION:        return "Chip revision";
            case Opt::VICII_GRAY_DOT_BUG:    return "Emulate gray-dot bug";
            case Opt::VICII_POWER_SAVE:      return "Take fast paths";
            case Opt::VICII_HIDE_SPRITES:    return "Hide sprites";
            case Opt::VICII_CUT_LAYERS:      return "Cut out graphics layers";
            case Opt::VICII_CUT_OPACITY:     return "Opacity";
            case Opt::VICII_SS_COLLISIONS:   return "Sprite-sprite collisions";
            case Opt::VICII_SB_COLLISIONS:   return "Sprite-background collisions";

            case Opt::DMA_DEBUG_ENABLE:      return "DMA Debugger";
            case Opt::DMA_DEBUG_OVERLAY:     return "Enable DMA Overlay";
            case Opt::DMA_DEBUG_MODE:        return "Overlay style";
            case Opt::DMA_DEBUG_OPACITY:     return "Overlay opacity";
            case Opt::DMA_DEBUG_CHANNEL0:    return "Refresh cycles";
            case Opt::DMA_DEBUG_CHANNEL1:    return "Idle reads";
            case Opt::DMA_DEBUG_CHANNEL2:    return "Character accesses";
            case Opt::DMA_DEBUG_CHANNEL3:    return "Graphics accesses";
            case Opt::DMA_DEBUG_CHANNEL4:    return "Sprite-pointer accesses";
            case Opt::DMA_DEBUG_CHANNEL5:    return "Sprite-data accesses";
            case Opt::DMA_DEBUG_COLOR0:      return "Color of channel 0";
            case Opt::DMA_DEBUG_COLOR1:      return "Color of channel 1";
            case Opt::DMA_DEBUG_COLOR2:      return "Color of channel 2";
            case Opt::DMA_DEBUG_COLOR3:      return "Color of channel 3";
            case Opt::DMA_DEBUG_COLOR4:      return "Color of channel 4";
            case Opt::DMA_DEBUG_COLOR5:      return "Color of channel 5";

            case Opt::EXP_REU_SPEED:         return "REU transfer speed";

            case Opt::USR_DEVICE:            return "User port device";

            case Opt::VID_WHITE_NOISE:       return "White noise";

            case Opt::MON_PALETTE:           return "Color palette";
            case Opt::MON_BRIGHTNESS:        return "Monitor brightness";
            case Opt::MON_CONTRAST:          return "Monitor contrast";
            case Opt::MON_SATURATION:        return "Monitor saturation";
            case Opt::MON_HCENTER:           return "Horizontal center";
            case Opt::MON_VCENTER:           return "Vertical center";
            case Opt::MON_HZOOM:             return "Horizontal zoom";
            case Opt::MON_VZOOM:             return "Vertical zoom";
            case Opt::MON_UPSCALER:          return "Upscaler";
            case Opt::MON_BLUR:              return "Blur";
            case Opt::MON_BLUR_RADIUS:       return "Blur radius";
            case Opt::MON_BLOOM:             return "Bloom";
            case Opt::MON_BLOOM_RADIUS:      return "Bloom radius";
            case Opt::MON_BLOOM_BRIGHTNESS:  return "Bloom brightness";
            case Opt::MON_BLOOM_WEIGHT:      return "Bloom weight";
            case Opt::MON_DOTMASK:           return "Dotmask";
            case Opt::MON_DOTMASK_BRIGHTNESS:    return "Dotmask brightness";
            case Opt::MON_SCANLINES:             return "Scanlines";
            case Opt::MON_SCANLINE_BRIGHTNESS:   return "Scanline brightness";
            case Opt::MON_SCANLINE_WEIGHT:       return "Scanline weight";
            case Opt::MON_DISALIGNMENT:      return "Cathode rays disalignment";
            case Opt::MON_DISALIGNMENT_H:    return "Horizontal cathode ray shift";
            case Opt::MON_DISALIGNMENT_V:    return "Vertical cathode ray shift";

            case Opt::POWER_GRID:            return "Power grid stability";
            case Opt::GLUE_LOGIC:            return "Glue-logic type";

            case Opt::CIA_REVISION:          return "Chip revision";
            case Opt::CIA_TIMER_B_BUG:       return "Emulate Timer-B bug";
            case Opt::CIA_IDLE_SLEEP:        return "Enter idle state while not in use";

            case Opt::SID_ENABLE:            return "Enable";
            case Opt::SID_ADDRESS:           return "Memory location";
            case Opt::SID_REV:          return "Chip revision";
            case Opt::SID_FILTER:            return "Use filter";
            case Opt::SID_ENGINE:            return "SID backend";
            case Opt::SID_SAMPLING:          return "Audio sampling rate";
            case Opt::SID_POWER_SAVE:        return "Take fast paths";
            case Opt::AUD_VOL0:              return "Channel 0 volume";
            case Opt::AUD_VOL1:              return "Channel 1 volume";
            case Opt::AUD_VOL2:              return "Channel 2 volume";
            case Opt::AUD_VOL3:              return "Channel 3 volume";
            case Opt::AUD_PAN0:              return "Channel 0 pan";
            case Opt::AUD_PAN1:              return "Channel 1 pan";
            case Opt::AUD_PAN2:              return "Channel 2 pan";
            case Opt::AUD_PAN3:              return "Channel 3 pan";
            case Opt::AUD_VOL_L:             return "Master volume (left)";
            case Opt::AUD_VOL_R:             return "Master volume (right)";

            case Opt::MEM_INIT_PATTERN:      return "Memory start-up pattern";
            case Opt::MEM_HEATMAP:           return "Heatmap";
            case Opt::MEM_SAVE_ROMS:         return "Save ROMs in snapshots";

            case Opt::DRV_AUTO_CONFIG:       return "Auto-configure drives";
            case Opt::DRV_TYPE:              return "Drive model";
            case Opt::DRV_RAM:               return "Drive RAM";
            case Opt::DRV_SAVE_ROMS:         return "Save Drive ROMs in snapshots";
            case Opt::DRV_PARCABLE:          return "Parallel cable";
            case Opt::DRV_CONNECT:           return "Connected";
            case Opt::DRV_POWER_SWITCH:      return "Power switch";
            case Opt::DRV_POWER_SAVE:        return "Take fast paths";
            case Opt::DRV_EJECT_DELAY:       return "Disk eject delay";
            case Opt::DRV_SWAP_DELAY:        return "Disk swap delay";
            case Opt::DRV_INSERT_DELAY:      return "Disk insertion delay";
            case Opt::DRV_PAN:               return "Pan";
            case Opt::DRV_POWER_VOL:         return "Power switch volume";
            case Opt::DRV_STEP_VOL:          return "Head step volume";
            case Opt::DRV_INSERT_VOL:        return "Disk insertion volume";
            case Opt::DRV_EJECT_VOL:         return "Disk ejection volume";

            case Opt::DAT_MODEL:             return "Datasette model";
            case Opt::DAT_CONNECT:           return "Datasette connected";

            case Opt::MOUSE_MODEL:           return "Mouse model";
            case Opt::MOUSE_SHAKE_DETECT:    return "Detect a shaked mouse";
            case Opt::MOUSE_VELOCITY:        return "Mouse velocity";

            case Opt::AUTOFIRE:              return "Autofire";
            case Opt::AUTOFIRE_BURSTS:       return "Burst mode";
            case Opt::AUTOFIRE_BULLETS:      return "Number of bullets per burst";
            case Opt::AUTOFIRE_DELAY:        return "Autofire delay in frames";

            case Opt::PADDLE_ORIENTATION:    return "Paddle value mapping scheme";

            case Opt::RS232_DEVICE:          return "Connected RS232 device";
            case Opt::RS232_BAUD:            return "Transmission rate (baud)";

            case Opt::REC_FRAME_RATE:        return "Frame rate of the recorded video";
            case Opt::REC_BIT_RATE:          return "Bit rate of the recorded video";
            case Opt::REC_SAMPLE_RATE:       return "Audio sample rate of the recorded video";
            case Opt::REC_ASPECT_X:          return "Numerator of the video's aspect ratio";
            case Opt::REC_ASPECT_Y:          return "Denumerator of the video's aspect ratio";

            case Opt::SRV_PORT:              return "Server port";
            case Opt::SRV_PROTOCOL:          return "Server protocol";
            case Opt::SRV_AUTORUN:           return "Auto run";
            case Opt::SRV_VERBOSE:           return "Verbose mode";

            case Opt::DBG_DEBUGCART:         return "VICE debug cartridge";
            case Opt::DBG_WATCHDOG:          return "Watchdog delay in cycles";

            case Opt::COUNT:                 return "???";
        }
        return "???";
    }
};

}
