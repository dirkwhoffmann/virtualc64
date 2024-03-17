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
#include "ThreadTypes.h"


//
// Enumerations
//

enum_long(DEBUG_FLAG)
{
    // General
    FLAG_XFILES,
    FLAG_CNF_DEBUG,
    FLAG_DEF_DEBUG,

    // Emulator
    FLAG_RUN_DEBUG,
    FLAG_TIM_DEBUG,
    FLAG_WARP_DEBUG,
    FLAG_CMD_DEBUG,
    FLAG_MSG_DEBUG,
    FLAG_SNP_DEBUG,

    // Run ahead
    FLAG_RUA_DEBUG,
    FLAG_RUA_ON_STEROIDS,

    // CPU
    FLAG_CPU_DEBUG,
    FLAG_IRQ_DEBUG,

    // Memory
    FLAG_MEM_DEBUG,

    // CIAs
    FLAG_CIA_DEBUG,
    FLAG_CIAREG_DEBUG,
    FLAG_CIA_ON_STEROIDS,

    // VICII
    FLAG_VICII_DEBUG,
    FLAG_VICII_REG_DEBUG,
    FLAG_VICII_SAFE_MODE,
    FLAG_VICII_STATS,
    FLAG_RASTERIRQ_DEBUG,

    // SID
    FLAG_SID_DEBUG,
    FLAG_SID_EXEC,
    FLAG_SIDREG_DEBUG,
    FLAG_AUDBUF_DEBUG,

    // Drive
    FLAG_VIA_DEBUG,
    FLAG_PIA_DEBUG,
    FLAG_IEC_DEBUG,
    FLAG_DSK_DEBUG,
    FLAG_DSKCHG_DEBUG,
    FLAG_GCR_DEBUG,
    FLAG_FS_DEBUG,
    FLAG_PAR_DEBUG,

    // Media
    FLAG_CRT_DEBUG,
    FLAG_FILE_DEBUG,

    // Peripherals
    FLAG_JOY_DEBUG,
    FLAG_DRV_DEBUG,
    FLAG_TAP_DEBUG,
    FLAG_KBD_DEBUG,
    FLAG_PRT_DEBUG,
    FLAG_EXP_DEBUG,
    FLAG_LIP_DEBUG,

    // Other components
    FLAG_REC_DEBUG,
    FLAG_REU_DEBUG,

    // Forced error conditions
    FLAG_FORCE_ROM_MISSING,
    FLAG_FORCE_MEGA64_MISMATCH,
    FLAG_FORCE_SNAP_TOO_OLD,
    FLAG_FORCE_SNAP_TOO_NEW,
    FLAG_FORCE_SNAP_IS_BETA,
    FLAG_FORCE_SNAP_CORRUPTED,
    FLAG_FORCE_CRT_UNKNOWN,
    FLAG_FORCE_CRT_UNSUPPORTED,
    FLAG_FORCE_RECORDING_ERROR,
    FLAG_FORCE_NO_FFMPEG
};
typedef DEBUG_FLAG DebugFlag;

enum_long(WARP_MODE)
{
    WARP_AUTO,
    WARP_NEVER,
    WARP_ALWAYS
};
typedef WARP_MODE WarpMode;


//
// Structures
//

typedef struct
{
    isize warpBoot;
    WarpMode warpMode;
    bool vsync;
    isize timeLapse;
    isize runAhead;
}
EmulatorConfig;

typedef struct
{
    EmulatorState state;
    isize refreshRate;
    bool powered;
    bool paused;
    bool running;
    bool suspended;
    bool warping;
    bool tracking;
}
EmulatorInfo;

typedef struct
{
    double cpuLoad;
    double fps;
}
EmulatorStats;
