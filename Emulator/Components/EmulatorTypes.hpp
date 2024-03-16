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

#include "EmulatorTypes.h"

struct DebugFlagEnum : util::Reflection<DebugFlagEnum, DebugFlag>
{
    static constexpr long minVal = 0;
    static constexpr long maxVal = FLAG_FORCE_NO_FFMPEG;
    static bool isValid(auto val) { return val >= minVal && val <= maxVal; }

    static const char *prefix() { return "FLAG"; }
    static const char *key(DebugFlag value)
    {
        switch (value) {

            case FLAG_XFILES:                   return "XFILES";
            case FLAG_CNF_DEBUG:                return "CNF_DEBUG";
            case FLAG_DEF_DEBUG:                return "DEF_DEBUG";

                // Emulator
            case FLAG_RUN_DEBUG:                return "RUN_DEBUG";
            case FLAG_TIM_DEBUG:                return "TIM_DEBUG";
            case FLAG_WARP_DEBUG:               return "WARP_DEBUG";
            case FLAG_CMD_DEBUG:                return "CMD_DEBUG";
            case FLAG_MSG_DEBUG:                return "MSG_DEBUG";
            case FLAG_SNP_DEBUG:                return "SNP_DEBUG";

                // Run ahead
            case FLAG_RUA_DEBUG:                return "RUA_DEBUG";
            case FLAG_RUA_ON_STEROIDS:          return "RUA_ON_STEROIDS";

                // CPU
            case FLAG_CPU_DEBUG:                return "CPU_DEBUG";
            case FLAG_IRQ_DEBUG:                return "IRQ_DEBUG";

                // Memory
            case FLAG_MEM_DEBUG:                return "MEM_DEBUG";

                // CIAs
            case FLAG_CIA_DEBUG:                return "CIA_DEBUG";
            case FLAG_CIAREG_DEBUG:             return "CIAREG_DEBUG";
            case FLAG_CIA_ON_STEROIDS:          return "CIA_ON_STEROIDS";

                // VICII
            case FLAG_VICII_DEBUG:              return "VICII_DEBUG";
            case FLAG_VICII_REG_DEBUG:          return "VICII_REG_DEBUG";
            case FLAG_VICII_SAFE_MODE:          return "VICII_SAFE_MODE";
            case FLAG_VICII_STATS:              return "VICII_STATS";
            case FLAG_RASTERIRQ_DEBUG:          return "RASTERIRQ_DEBUG";

                // SID
            case FLAG_SID_DEBUG:                return "SID_DEBUG";
            case FLAG_SID_EXEC:                 return "SID_EXEC";
            case FLAG_SIDREG_DEBUG:             return "SIDREG_DEBUG";
            case FLAG_AUDBUF_DEBUG:             return "AUDBUF_DEBUG";

                // Drive
            case FLAG_VIA_DEBUG:                return "VIA_DEBUG";
            case FLAG_PIA_DEBUG:                return "PIA_DEBUG";
            case FLAG_IEC_DEBUG:                return "IEC_DEBUG";
            case FLAG_DSK_DEBUG:                return "DSK_DEBUG";
            case FLAG_DSKCHG_DEBUG:             return "DSKCHG_DEBUG";
            case FLAG_GCR_DEBUG:                return "GCR_DEBUG";
            case FLAG_FS_DEBUG:                 return "FS_DEBUG";
            case FLAG_PAR_DEBUG:                return "PAR_DEBUG";

                // Media
            case FLAG_CRT_DEBUG:                return "CRT_DEBUG";
            case FLAG_FILE_DEBUG:               return "FILE_DEBUG";

                // Peripherals
            case FLAG_JOY_DEBUG:                return "JOY_DEBUG";
            case FLAG_DRV_DEBUG:                return "DRV_DEBUG";
            case FLAG_TAP_DEBUG:                return "TAP_DEBUG";
            case FLAG_KBD_DEBUG:                return "KBD_DEBUG";
            case FLAG_PRT_DEBUG:                return "PRT_DEBUG";
            case FLAG_EXP_DEBUG:                return "EXP_DEBUG";
            case FLAG_LIP_DEBUG:                return "LIP_DEBUG";

                // Other components
            case FLAG_REC_DEBUG:                return "REC_DEBUG";
            case FLAG_REU_DEBUG:                return "REU_DEBUG";

                // Forced error conditions
            case FLAG_FORCE_ROM_MISSING:        return "FORCE_ROM_MISSING";
            case FLAG_FORCE_MEGA64_MISMATCH:    return "FORCE_MEGA64_MISMATCH";
            case FLAG_FORCE_SNAP_TOO_OLD:       return "FORCE_SNAP_TOO_OLD";
            case FLAG_FORCE_SNAP_TOO_NEW:       return "FORCE_SNAP_TOO_NEW";
            case FLAG_FORCE_SNAP_IS_BETA:       return "FORCE_SNAP_IS_BETA";
            case FLAG_FORCE_SNAP_CORRUPTED:     return "FORCE_SNAP_CORRUPTE";
            case FLAG_FORCE_CRT_UNKNOWN:        return "FORCE_CRT_UNKNOWN";
            case FLAG_FORCE_CRT_UNSUPPORTED:    return "FORCE_CRT_UNSUPPORTED";
            case FLAG_FORCE_RECORDING_ERROR:    return "FORCE_RECORDING_ERROR";
            case FLAG_FORCE_NO_FFMPEG:          return "FORCE_NO_FFMPEG";
        }
        return "???";
    }

    static const char *help(DebugFlag value)
    {
        switch (value) {

            case FLAG_XFILES:                   return "Report paranormal activity";
            case FLAG_CNF_DEBUG:                return "Configuration";
            case FLAG_DEF_DEBUG:                return "User defaults";

                // Emulator
            case FLAG_RUN_DEBUG:                return "Run loop, component states";
            case FLAG_TIM_DEBUG:                return "Thread synchronization";
            case FLAG_WARP_DEBUG:               return "Warp mode";
            case FLAG_CMD_DEBUG:                return "Command queue";
            case FLAG_MSG_DEBUG:                return "Message queue";
            case FLAG_SNP_DEBUG:                return "Serializing (snapshots)";

                // Run ahead
            case FLAG_RUA_DEBUG:                return "Run-ahead mode";
            case FLAG_RUA_ON_STEROIDS:          return "Update the run-ahead instance in every frame";

                // CPU
            case FLAG_CPU_DEBUG:                return "CPU";
            case FLAG_IRQ_DEBUG:                return "Interrupts";

                // Memory
            case FLAG_MEM_DEBUG:                return "RAM and ROM";

                // CIAs
            case FLAG_CIA_DEBUG:                return "Complex Interface Adapter";
            case FLAG_CIAREG_DEBUG:             return "CIA registers";
            case FLAG_CIA_ON_STEROIDS:          return "Keep CIAs awake all the time";

                // VICII
            case FLAG_VICII_DEBUG:              return "Video Interface Controller";
            case FLAG_VICII_REG_DEBUG:          return "VICII registers";
            case FLAG_VICII_SAFE_MODE:          return "Disable performance optimizations";
            case FLAG_VICII_STATS:              return "Collect statistical information";
            case FLAG_RASTERIRQ_DEBUG:          return "Raster interrupt";

                // SID
            case FLAG_SID_DEBUG:                return "Sound Interface Device";
            case FLAG_SID_EXEC:                 return "Sound Interface Device (Execution)";
            case FLAG_SIDREG_DEBUG:             return "Sound Interface Device (Registers)";
            case FLAG_AUDBUF_DEBUG:             return "Audio buffers";

                // Drive
            case FLAG_VIA_DEBUG:                return "Versatile Interface Adapter";
            case FLAG_PIA_DEBUG:                return "Peripheral Interface Adapter";
            case FLAG_IEC_DEBUG:                return "IEC bus";
            case FLAG_DSK_DEBUG:                return "Disk controller execution";
            case FLAG_DSKCHG_DEBUG:             return "Disk change procedure";
            case FLAG_GCR_DEBUG:                return "Disk encoding / decoding";
            case FLAG_FS_DEBUG:                 return "File System Classes";
            case FLAG_PAR_DEBUG:                return "Parallel cable";

                // Media
            case FLAG_CRT_DEBUG:                return "Cartridges";
            case FLAG_FILE_DEBUG:               return "Media files (D64,T64,...)";

                // Peripherals
            case FLAG_JOY_DEBUG:                return "Joystick";
            case FLAG_DRV_DEBUG:                return "Floppy drives";
            case FLAG_TAP_DEBUG:                return "Datasette";
            case FLAG_KBD_DEBUG:                return "Keyboard";
            case FLAG_PRT_DEBUG:                return "Control ports and connected devices";
            case FLAG_EXP_DEBUG:                return "Expansion port";
            case FLAG_LIP_DEBUG:                return "Light pen";

                // Other components
            case FLAG_REC_DEBUG:                return "Screen recorder";
            case FLAG_REU_DEBUG:                return "REU memory expansion";

                // Forced error conditions
            case FLAG_FORCE_ROM_MISSING:        return "";
            case FLAG_FORCE_MEGA64_MISMATCH:    return "";
            case FLAG_FORCE_SNAP_TOO_OLD:       return "";
            case FLAG_FORCE_SNAP_TOO_NEW:       return "";
            case FLAG_FORCE_SNAP_IS_BETA:       return "";
            case FLAG_FORCE_SNAP_CORRUPTED:     return "";
            case FLAG_FORCE_CRT_UNKNOWN:        return "";
            case FLAG_FORCE_CRT_UNSUPPORTED:    return "";
            case FLAG_FORCE_RECORDING_ERROR:    return "";
            case FLAG_FORCE_NO_FFMPEG:          return "";
        }
        return "???";
    }
};

struct WarpModeEnum : util::Reflection<WarpModeEnum, WarpMode>
{
    static constexpr long minVal = 0;
    static constexpr long maxVal = WARP_ALWAYS;
    static bool isValid(auto val) { return val >= minVal && val <= maxVal; }

    static const char *prefix() { return "WARP"; }
    static const char *key(WarpMode value)
    {
        switch (value) {

            case WARP_AUTO:     return "WARP_AUTO";
            case WARP_NEVER:    return "WARP_NEVER";
            case WARP_ALWAYS:   return "WARP_ALWAYS";
        }
        return "???";
    }
};
