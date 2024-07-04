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

// General
debugflag XFILES          = 0;
debugflag CNF_DEBUG       = 0;
debugflag DEF_DEBUG       = 0;

// Runloop
debugflag RUN_DEBUG       = 0;
debugflag TIM_DEBUG       = 0;
debugflag WARP_DEBUG      = 0;
debugflag CMD_DEBUG       = 0;
debugflag MSG_DEBUG       = 0;
debugflag SNP_DEBUG       = 0;

// Run ahead
debugflag RUA_DEBUG       = 0;
debugflag RUA_ON_STEROIDS = 0;

// CPU
debugflag CPU_DEBUG       = 0;
debugflag IRQ_DEBUG       = 0;

// Memory
debugflag MEM_DEBUG       = 0;

// CIAs
debugflag CIA_DEBUG       = 0;
debugflag CIAREG_DEBUG    = 0;
debugflag CIA_ON_STEROIDS = 0;

// VICII
debugflag VICII_DEBUG     = 0;
debugflag VICII_REG_DEBUG = 0;
debugflag VICII_SAFE_MODE = 0;
debugflag VICII_STATS     = 0;
debugflag RASTERIRQ_DEBUG = 0;

// SID
debugflag SID_DEBUG       = 0;
debugflag SID_EXEC        = 0;
debugflag SIDREG_DEBUG    = 0;
debugflag AUDBUF_DEBUG    = 0;
debugflag AUDVOL_DEBUG    = 0;

// Drive
debugflag VIA_DEBUG       = 0;
debugflag PIA_DEBUG       = 0;
debugflag SER_DEBUG       = 0;
debugflag DSK_DEBUG       = 0;
debugflag DSKCHG_DEBUG    = 0;
debugflag GCR_DEBUG       = 0;
debugflag FS_DEBUG        = 0;
debugflag PAR_DEBUG       = 0;

// Media
debugflag CRT_DEBUG       = 0;
debugflag FILE_DEBUG      = 0;

// Peripherals
debugflag JOY_DEBUG       = 0;
debugflag DRV_DEBUG       = 0;
debugflag TAP_DEBUG       = 0;
debugflag KBD_DEBUG       = 0;
debugflag PRT_DEBUG       = 0;
debugflag EXP_DEBUG       = 0;
debugflag USR_DEBUG       = 0;

// Other components
debugflag REC_DEBUG       = 0;
debugflag REU_DEBUG       = 0;


//
// Forced error conditions
//

debugflag FORCE_ROM_MISSING              = 0;
debugflag FORCE_MEGA64_MISMATCH          = 0;
debugflag FORCE_SNAP_TOO_OLD             = 0;
debugflag FORCE_SNAP_TOO_NEW             = 0;
debugflag FORCE_SNAP_IS_BETA             = 0;
debugflag FORCE_SNAP_CORRUPTED           = 0;
debugflag FORCE_CRT_UNKNOWN              = 0;
debugflag FORCE_CRT_UNSUPPORTED          = 0;
debugflag FORCE_RECORDING_ERROR          = 0;
debugflag FORCE_NO_FFMPEG                = 0;
