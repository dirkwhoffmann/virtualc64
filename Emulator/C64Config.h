// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

//
// Release settings
//

// Snapshot version number
#define V_MAJOR 4
#define V_MINOR 0
#define V_SUBMINOR 1

// Uncomment these settings in a release build
// #define RELEASEBUILD
// #define NDEBUG

//
// Configuration overrides
//

// Uncomment to override a configuration setting
// EMPTY LIST SO FAR


//
// Debug settings
//

// General
static const int CNF_DEBUG       = 0; // Configuration
static const int XFILES          = 0; // Report paranormal activity

// Runloop
static const int RUN_DEBUG       = 0; // Run loop, component states, timing
static const int TIM_DEBUG       = 0; // Timing (thread synchronization)
static const int SNP_DEBUG       = 0; // Serializing (snapshots)
static const int MSG_DEBUG       = 0; // Message queue

// CPU
static const int CPU_DEBUG       = 0; // CPU
static const int IRQ_DEBUG       = 0; // Interrupts

// Memory
static const int MEM_DEBUG       = 1; // RAM, ROM

// CIAs
static const int CIA_DEBUG       = 0; // Complex Interface Adapter
static const int CIA_ON_STEROIDS = 0; // Keep the CIAs awake all the time

// Custom chips
static const int VIA_DEBUG       = 0; // Versatile Interface Adapter
static const int VIC_DEBUG       = 0; // Video Interface Controller
static const int SID_DEBUG       = 0; // Sound Interface Device
static const int SID_EXEC        = 0; // Sound Interface Device (execution)
static const int IEC_DEBUG       = 0; // IEC bus

// Ports
static const int PORT_DEBUG      = 0; // Control ports and connected devices
static const int EXP_DEBUG       = 0; // Expansion port

// Drive
static const int DSK_DEBUG       = 0; // Disk controller execution
static const int DSKCHG_DEBUG    = 0; // Disk changing procedure
static const int GCR_DEBUG       = 0; // Disk encoding / decoding
static const int FS_DEBUG        = 0; // File System Classes

// Media
static const int CRT_DEBUG       = 0; // Cartridges
static const int FILE_DEBUG      = 1; // Media files (D64,T64,...)

// Peripherals
static const int JOY_DEBUG       = 0; // Joystick
static const int MOUSE_DEBUG     = 0; // Mouse
static const int DRV_DEBUG       = 0; // Floppy drive
static const int TAP_DEBUG       = 0; // Datasette
static const int KBD_DEBUG       = 0; // Keyboard


// Default debug level for all components (Set to 1 in release build)
#define DEBUG_LEVEL 1


// CLEANUP:
//
// OPTIMIZATION:
//
// Check, how -Ofast compares to -O3
// Check how USE_OPTIMIZATION_PROFILE = true influences runtime
//
// 
// Update IEC bus inside CIA and VIA. Use delay flags if neccessary
// Use a simpler implementation for the raster irq trigger. Edge sensitive matching value
// Call CA1 action in VIA class only if the pin value really has changed.
