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

#pragma once

//
// Release settings
//

// Version number
static constexpr int VER_MAJOR      = 5;
static constexpr int VER_MINOR      = 3;
static constexpr int VER_SUBMINOR   = 0;
static constexpr int VER_BETA       = 0;

// Snapshot version number
static constexpr int SNP_MAJOR      = 5;
static constexpr int SNP_MINOR      = 3;
static constexpr int SNP_SUBMINOR   = 0;
static constexpr int SNP_BETA       = 0;


//
// Configuration overrides
//

#define OVERRIDES { }


//
// Debug settings
//

static constexpr bool betaRelease = VER_BETA != 0;

#ifdef NDEBUG
static constexpr bool releaseBuild = 1;
static constexpr bool debugBuild = 0;
typedef const int debugflag;
#else
static constexpr bool releaseBuild = 0;
static constexpr bool debugBuild = 1;
typedef int debugflag;
#endif

#ifdef __EMSCRIPTEN__
static constexpr bool emscripten = 1;
#else
static constexpr bool emscripten = 0;
#endif

namespace vc64 {

// General
extern debugflag XFILES;
extern debugflag CNF_DEBUG;
extern debugflag DEF_DEBUG;

// Emulator
extern debugflag RUN_DEBUG;
extern debugflag TIM_DEBUG;
extern debugflag WARP_DEBUG;
extern debugflag CMD_DEBUG;
extern debugflag MSG_DEBUG;
extern debugflag SNP_DEBUG;

// Run ahead
extern debugflag RUA_DEBUG;
extern debugflag RUA_CHECKSUM;
extern debugflag RUA_ON_STEROIDS;

// CPU
extern debugflag CPU_DEBUG;
extern debugflag IRQ_DEBUG;

// Memory
extern debugflag MEM_DEBUG;

// CIAs
extern debugflag CIA_DEBUG;
extern debugflag CIAREG_DEBUG;

// VICII
extern debugflag VICII_DEBUG;
extern debugflag VICII_REG_DEBUG;
extern debugflag VICII_SAFE_MODE;
extern debugflag VICII_STATS;
extern debugflag RASTERIRQ_DEBUG;

// SID
extern debugflag SID_DEBUG;
extern debugflag SID_EXEC;
extern debugflag SIDREG_DEBUG;
extern debugflag AUDBUF_DEBUG;
extern debugflag AUDVOL_DEBUG;

// Drive
extern debugflag VIA_DEBUG;
extern debugflag PIA_DEBUG;
extern debugflag SER_DEBUG;
extern debugflag DSK_DEBUG;
extern debugflag DSKCHG_DEBUG;
extern debugflag GCR_DEBUG;
extern debugflag FS_DEBUG;
extern debugflag PAR_DEBUG;

// Media
extern debugflag CRT_DEBUG;
extern debugflag FILE_DEBUG;

// Peripherals
extern debugflag JOY_DEBUG;
extern debugflag DRV_DEBUG;
extern debugflag TAP_DEBUG;
extern debugflag KBD_DEBUG;
extern debugflag PRT_DEBUG;
extern debugflag EXP_DEBUG;
extern debugflag USR_DEBUG;

// Other components
extern debugflag REC_DEBUG;
extern debugflag REU_DEBUG;
extern debugflag SCK_DEBUG;
extern debugflag SRV_DEBUG;


//
// Forced error conditions
//

extern debugflag FORCE_LAUNCH_ERROR;
extern debugflag FORCE_ROM_MISSING;
extern debugflag FORCE_MEGA64_MISMATCH;
extern debugflag FORCE_SNAP_TOO_OLD;
extern debugflag FORCE_SNAP_TOO_NEW;
extern debugflag FORCE_SNAP_IS_BETA;
extern debugflag FORCE_SNAP_CORRUPTED;
extern debugflag FORCE_CRT_UNKNOWN;
extern debugflag FORCE_CRT_UNSUPPORTED;
extern debugflag FORCE_RECORDING_ERROR;
extern debugflag FORCE_NO_FFMPEG;
extern debugflag FORCE_ZLIB_ERROR;

}

#include <assert.h>
