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
#define VER_MAJOR 5
#define VER_MINOR 0
#define VER_SUBMINOR 0
#define VER_BETA 6

// Snapshot version number
#define SNP_MAJOR 5
#define SNP_MINOR 0
#define SNP_SUBMINOR 0
#define SNP_BETA 6

// Uncomment these settings in a release build
// #define RELEASEBUILD


//
// Build settings
//

#if defined(__clang__)

#define alwaysinline __attribute__((always_inline))
#pragma GCC diagnostic ignored "-Wgnu-anonymous-struct"
#pragma GCC diagnostic ignored "-Wnested-anon-types"

#elif defined(__GNUC__) || defined(__GNUG__)

#define alwaysinline __attribute__((always_inline))

#elif defined(_MSC_VER)

#define alwaysinline __forceinline

#endif


//
// Configuration overrides
//

#define OVERRIDES { }


//
// Debug settings
//

#ifdef RELEASEBUILD
#ifndef NDEBUG
#define NDEBUG
#endif
static const bool releaseBuild = 1;
static const bool debugBuild = 0;
typedef const int debugflag;
#else
static const bool releaseBuild = 0;
static const bool debugBuild = 1;
typedef int debugflag;
#endif

#if VER_BETA == 0
static const bool betaRelease = 0;
#else
static const bool betaRelease = 1;
#endif

#ifdef __EMSCRIPTEN__
static const bool emscripten = 1;
#else
static const bool emscripten = 0;
#endif


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
extern debugflag RUA_ON_STEROIDS;

// CPU
extern debugflag CPU_DEBUG;
extern debugflag IRQ_DEBUG;

// Memory
extern debugflag MEM_DEBUG;

// CIAs
extern debugflag CIA_DEBUG;
extern debugflag CIAREG_DEBUG;
extern debugflag CIA_ON_STEROIDS;

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
extern debugflag LIP_DEBUG;

// Other components
extern debugflag REC_DEBUG;
extern debugflag REU_DEBUG;


//
// Forced error conditions
//

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

#include <assert.h>
