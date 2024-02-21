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
debugflag XFILES          = 0; // Report paranormal activity
debugflag CNF_DEBUG       = 0; // Configuration
debugflag DEF_DEBUG       = 0; // User defaults

// Runloop
debugflag RUN_DEBUG       = 0; // Run loop, component states
debugflag TIM_DEBUG       = 0; // Thread synchronization
debugflag WARP_DEBUG      = 0; // Warp mode
debugflag CMD_DEBUG       = 0; // Command queue
debugflag MSG_DEBUG       = 0; // Message queue
debugflag SNP_DEBUG       = 0; // Serializing (snapshots)

// Run ahead
debugflag RUA_DEBUG       = 1; // Run ahead
debugflag RUA_TEXTURE     = 0; // 0 = Auto, 1 = Original, 2 = Run ahead
debugflag RUA_ON_STEROIDS = 0; // Update the run-ahead instance in every frame

// CPU
debugflag CPU_DEBUG       = 0; // CPU
debugflag IRQ_DEBUG       = 0; // Interrupts

// Memory
debugflag MEM_DEBUG       = 0; // RAM, ROM

// CIAs
debugflag CIA_DEBUG       = 0; // Complex Interface Adapter
debugflag CIAREG_DEBUG    = 0; // CIA registers
debugflag CIA_ON_STEROIDS = 0; // Keep the CIAs awake all the time

// VICII
debugflag VIC_DEBUG       = 0; // Video Interface Controller II
debugflag VICREG_DEBUG    = 0; // VICII registers
debugflag RASTERIRQ_DEBUG = 0; // Raster interrupt
debugflag VIC_SAFE_MODE   = 0; // Disable performance optimizations
debugflag VIC_STATS       = 0; // Collect statistical information

// SID
debugflag SID_DEBUG       = 0; // Sound Interface Device
debugflag SID_EXEC        = 0; // Sound Interface Device (Execution)
debugflag SIDREG_DEBUG    = 0; // Sound Interface Device (Registers)
debugflag AUDBUF_DEBUG    = 0; // Audio buffers

// Drive
debugflag VIA_DEBUG       = 0; // Versatile Interface Adapter
debugflag PIA_DEBUG       = 0; // Peripheral Interface Adapter
debugflag IEC_DEBUG       = 0; // IEC bus
debugflag DSK_DEBUG       = 0; // Disk controller execution
debugflag DSKCHG_DEBUG    = 0; // Disk changing procedure
debugflag GCR_DEBUG       = 0; // Disk encoding / decoding
debugflag FS_DEBUG        = 0; // File System Classes
debugflag PAR_DEBUG       = 0; // Parallel cable

// Media
debugflag CRT_DEBUG       = 0; // Cartridges
debugflag FILE_DEBUG      = 0; // Media files (D64,T64,...)

// Peripherals
debugflag JOY_DEBUG       = 0; // Joystick
debugflag DRV_DEBUG       = 0; // Floppy drive
debugflag TAP_DEBUG       = 0; // Datasette
debugflag KBD_DEBUG       = 0; // Keyboard
debugflag PRT_DEBUG       = 0; // Control ports and connected devices
debugflag EXP_DEBUG       = 0; // Expansion port
debugflag LIP_DEBUG       = 0; // Light pen

// Other components
debugflag REC_DEBUG       = 0; // Screen recorder
debugflag REU_DEBUG       = 0; // REU memory expansion


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

