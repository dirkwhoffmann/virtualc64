// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

//
// Release settings
//

// Version number
#define VER_MAJOR 4
#define VER_MINOR 5
#define VER_SUBMINOR 0

// Snapshot version number
#define SNP_MAJOR 4
#define SNP_MINOR 5
#define SNP_SUBMINOR 0

// Uncomment these settings in a release build
// #define RELEASEBUILD


//
// Build settings
//

#pragma GCC diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#pragma GCC diagnostic ignored "-Wgnu-anonymous-struct"
#pragma GCC diagnostic ignored "-Wnested-anon-types"


//
// Configuration overrides
//

#define OVERRIDES { }

// Uncomment to fallback to an older style thread messaging mechanism
// #define USE_CONDITION_VARIABLE


//
// Debug settings
//

// General
static const int CNF_DEBUG       = 0; // Configuration
static const int XFILES          = 0; // Report paranormal activity

// Runloop
static const int RUN_DEBUG       = 0; // Run loop, component states, timing
static const int WARP_DEBUG      = 0; // Warp mode
static const int QUEUE_DEBUG     = 0; // Message queue
static const int SNP_DEBUG       = 0; // Serializing (snapshots)

// CPU
static const int CPU_DEBUG       = 0; // CPU
static const int IRQ_DEBUG       = 0; // Interrupts

// Memory
static const int MEM_DEBUG       = 0; // RAM, ROM

// CIAs
static const int CIA_DEBUG       = 0; // Complex Interface Adapter
static const int CIAREG_DEBUG    = 0; // CIA registers
static const int CIA_ON_STEROIDS = 0; // Keep the CIAs awake all the time

// VICII
static const int VIC_DEBUG       = 0; // Video Interface Controller II
static const int VICREG_DEBUG    = 0; // VICII registers
static const int RASTERIRQ_DEBUG = 0; // Raster interrupt
static const int VIC_SAFE_MODE   = 0; // Disable performance optimizations
static const int VIC_STATS       = 0; // Collect statistical information

// SID
static const int SID_DEBUG       = 0; // Sound Interface Device
static const int SID_EXEC        = 0; // Sound Interface Device (Execution)
static const int SIDREG_DEBUG    = 0; // Sound Interface Device (Registers)
static const int AUDBUF_DEBUG    = 0; // Audio buffers

// Drive
static const int VIA_DEBUG       = 0; // Versatile Interface Adapter
static const int PIA_DEBUG       = 0; // Peripheral Interface Adapter
static const int IEC_DEBUG       = 0; // IEC bus
static const int DSK_DEBUG       = 0; // Disk controller execution
static const int DSKCHG_DEBUG    = 0; // Disk changing procedure
static const int GCR_DEBUG       = 0; // Disk encoding / decoding
static const int FS_DEBUG        = 0; // File System Classes
static const int PAR_DEBUG       = 0; // Parallel cable

// Media
static const int CRT_DEBUG       = 0; // Cartridges
static const int FILE_DEBUG      = 0; // Media files (D64,T64,...)

// Peripherals
static const int JOY_DEBUG       = 0; // Joystick
static const int DRV_DEBUG       = 0; // Floppy drive
static const int TAP_DEBUG       = 0; // Datasette
static const int KBD_DEBUG       = 0; // Keyboard
static const int PRT_DEBUG       = 0; // Control ports and connected devices
static const int EXP_DEBUG       = 0; // Expansion port
static const int LIP_DEBUG       = 0; // Light pen

// Other components
static const int REC_DEBUG       = 0; // Screen recorder


//
// Forced error conditions
//

static const int FORCE_ROM_MISSING      = 0;
static const int FORCE_MEGA64_MISMATCH  = 0;
static const int FORCE_CRT_UNKNOWN      = 0;
static const int FORCE_CRT_UNSUPPORTED  = 0;
static const int FORCE_SNAPSHOT_TOO_OLD = 0;
static const int FORCE_SNAPSHOT_TOO_NEW = 0;


#ifdef RELEASEBUILD
static const int releaseBuild = 1;
#define NDEBUG
#else
static const int releaseBuild = 0;
#endif

#include <assert.h>


// IDEAS:
// Update IEC bus inside CIA and VIA. Use delay flags if neccessary.
// Call CA1 action in VIA class only if the pin value really has changed.
