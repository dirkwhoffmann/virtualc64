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
#include "utl/abilities/Loggable.h"

#define STR(x) #x
#define XSTR(x) STR(x)

#define DEBUG_CHANNEL(name, description) \
LogChannel name = \
  ::utl::debug::name ? \
    Loggable::subscribe(XSTR(name), std::optional<long>(7), description) : \
    Loggable::subscribe(XSTR(name), std::optional<long>(std::nullopt), description);

namespace utl::channel {

// Register the default channels
DEBUG_CHANNEL(NULLDEV,          "Message sink");
DEBUG_CHANNEL(STDERR,           "Standard error");

// Register a logging channel for each debug flag

// General
DEBUG_CHANNEL(XFILES,           "Report paranormal activity")
DEBUG_CHANNEL(CNF_DEBUG,        "Configuration options")
DEBUG_CHANNEL(DEF_DEBUG,        "User defaults")

// Emulator
DEBUG_CHANNEL(RUN_DEBUG,        "Run loop, component states");
DEBUG_CHANNEL(TIM_DEBUG,        "Thread synchronization");
DEBUG_CHANNEL(WARP_DEBUG,       "Warp mode");
DEBUG_CHANNEL(CMD_DEBUG,        "Command queue");
DEBUG_CHANNEL(MSG_DEBUG,        "Message queue");
DEBUG_CHANNEL(SNP_DEBUG,        "Serialization (snapshots)");

// Run ahead
DEBUG_CHANNEL(RUA_DEBUG,        "Run-ahead activit");
DEBUG_CHANNEL(RUA_CHECKSUM,     "Run-ahead instance integrity");
DEBUG_CHANNEL(RUA_ON_STEROIDS,  "Update RUA instance every frame");

// CPU
DEBUG_CHANNEL(CPU_DEBUG,        "CPU");
DEBUG_CHANNEL(IRQ_DEBUG,        "CPU Interrupts");

// Memory
DEBUG_CHANNEL(MEM_DEBUG,        "Memory");

// CIAs
DEBUG_CHANNEL(CIA_DEBUG,        "CIA execution");
DEBUG_CHANNEL(CIAREG_DEBUG,     "CIA registers");

// VICII
DEBUG_CHANNEL(VICII_DEBUG,      "VICII execution");
DEBUG_CHANNEL(VICII_REG_DEBUG,  "VICII registers");
DEBUG_CHANNEL(VICII_SAFE_MODE,  "VICII optimizations");
DEBUG_CHANNEL(VICII_STATS,      "VICII statistics");
DEBUG_CHANNEL(RASTERIRQ_DEBUG,  "VICII raster IRQ");

// SID
DEBUG_CHANNEL(SID_DEBUG,        "SID");
DEBUG_CHANNEL(SID_EXEC,         "SID execution");
DEBUG_CHANNEL(SIDREG_DEBUG,     "SID registers");
DEBUG_CHANNEL(AUD_DEBUG,        "Sound generation");
DEBUG_CHANNEL(AUDBUF_DEBUG,     "Audio buffers");
DEBUG_CHANNEL(AUDVOL_DEBUG,     "Audio volume");

// Drive
DEBUG_CHANNEL(VIA_DEBUG,        "VIA chip");
DEBUG_CHANNEL(PIA_DEBUG,        "PIA chip");
DEBUG_CHANNEL(SER_DEBUG,        "Serial port (IEC bus)");
DEBUG_CHANNEL(DSK_DEBUG,        "Disk");
DEBUG_CHANNEL(DSKCHG_DEBUG,     "Disk change procedure");
DEBUG_CHANNEL(GCR_DEBUG,        "GCR encoding");
DEBUG_CHANNEL(FS_DEBUG,         "File system");
DEBUG_CHANNEL(PAR_DEBUG,        "Parallel port");

// Media
DEBUG_CHANNEL(IMG_DEBUG,        "Disk images");
DEBUG_CHANNEL(CRT_DEBUG,        "Cartridges");
DEBUG_CHANNEL(FILE_DEBUG,       "Media files");

// Peripherals
DEBUG_CHANNEL(JOY_DEBUG,        "Joystick");
DEBUG_CHANNEL( DRV_DEBUG,       "Floppy drive");
DEBUG_CHANNEL(TAP_DEBUG,        "Datasette");
DEBUG_CHANNEL(KBD_DEBUG,        "Keyboard");
DEBUG_CHANNEL(PRT_DEBUG,        "Ports");
DEBUG_CHANNEL(EXP_DEBUG,        "Expansion port");
DEBUG_CHANNEL(USR_DEBUG,        "User port");

// Misc
DEBUG_CHANNEL(RSH_DEBUG,        "RetroShell");
DEBUG_CHANNEL(REC_DEBUG,        "DEPRECATED");
DEBUG_CHANNEL(REU_DEBUG,        "REU drive");
DEBUG_CHANNEL(SCK_DEBUG,        "Sockets");
DEBUG_CHANNEL(SRV_DEBUG,        "Servers");
DEBUG_CHANNEL(GDB_DEBUG,        "GDB server");
DEBUG_CHANNEL(DAP_DEBUG,        "DAP server");

}
