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

#include "utl/abilities/Loggable.h"
#include <source_location>

#ifdef NDEBUG
#define CONSTEXPR constexpr
#else
#define CONSTEXPR
#endif

/* How the debug system works:
 *
 * In release builds (NDEBUG), CONSTEXPR expands to 'constexpr'. Every debug
 * flag below then becomes a compile-time constant, and 'if CONSTEXPR' below
 * becomes 'if constexpr', so the compiler removes each guarded log call (or
 * debug action) entirely when its flag is off.
 *
 * In debug builds, CONSTEXPR expands to nothing. The same flags become
 * ordinary (inline) variables that can be switched on and off at runtime,
 * either by editing the tables below or via RetroShell ('log' and 'debug').
 *
 * Debug flags come in two kinds, declared in the two tables below:
 *
 * - Logging flags gate a logme() call and nothing else. Disabling one
 *   removes the call (release builds) or simply keeps it silent (debug
 *   builds); the emulator behaves identically either way. They are typed
 *   'long', because their value doubles as the severity the message is
 *   issued with. LOG_OFF disables the call, any other severity enables it
 *   at that level.
 *
 * - Debug flags enable extra behavior with a real side effect (an
 *   integrity check, a redundant computation compared against the fast
 *   path, forcing a specific code path, simulating an error condition,
 *   ...). Some of these also log a message as part of that action, but
 *   disabling the flag changes what the emulator *does*, not just what
 *   it prints. They are all typed 'bool'.
 *
 * Both tables are X-macro lists: each entry names a flag exactly once, and
 * is expanded both into the variable declaration and (in debug builds) into
 * a descriptor table used by RetroShell. To add a flag, add one line here.
 *
 * Note that rvlib maintains its own, independent set of flags (rvdebug.h).
 */

namespace vc64 {

using utl::FlagInfo;

inline constexpr long LOG_OFF     = 0;
inline constexpr long LOG_FATAL   = 1;
inline constexpr long LOG_ERROR   = 2;
inline constexpr long LOG_WARNING = 3;
inline constexpr long LOG_INFO    = 4;
inline constexpr long LOG_DEBUG   = 5;
inline constexpr long LOG_TRACE   = 6;


//
// Logging flags
//

#define VC_LOG_FLAGS(E)                                                       \
                                                                              \
    /* General */                                                             \
    E(LOG_XFILES,       LOG_OFF,  "Report paranormal activity")             \
    E(LOG_CNF,          LOG_OFF,  "Configuration options")                  \
    E(LOG_DEF,          LOG_OFF,  "User defaults")                          \
                                                                              \
    /* Runloop */                                                             \
    E(LOG_RUN,          LOG_OFF,  "Run loop, component states")             \
    E(LOG_TIM,          LOG_OFF,  "Thread synchronization")                 \
    E(LOG_WARP,         LOG_OFF,  "Warp mode")                              \
    E(LOG_CMD,          LOG_OFF,  "Command queue")                          \
    E(LOG_MSG,          LOG_OFF,  "Message queue")                          \
    E(LOG_SNP,          LOG_OFF,  "Serialization (snapshots)")              \
                                                                              \
    /* Run ahead */                                                           \
    E(LOG_RUA,          LOG_OFF,  "Run-ahead activity")                     \
                                                                              \
    /* CPU */                                                                 \
    E(LOG_CPU,          LOG_OFF,  "CPU")                                    \
    E(LOG_IRQ,          LOG_OFF,  "CPU interrupts")                         \
                                                                              \
    /* Memory */                                                              \
    E(LOG_MEM,          LOG_OFF,  "Memory")                                 \
                                                                              \
    /* CIAs */                                                                \
    E(LOG_CIA,          LOG_OFF,  "CIA execution")                          \
    E(LOG_CIAREG,       LOG_OFF,  "CIA registers")                          \
                                                                              \
    /* VICII */                                                               \
    E(LOG_VICII,        LOG_OFF,  "VICII execution")                        \
    E(LOG_VICII_REG,    LOG_OFF,  "VICII registers")                        \
    E(LOG_RASTERIRQ,    LOG_OFF,  "VICII raster IRQ")                       \
                                                                              \
    /* SID */                                                                 \
    E(LOG_SID,          LOG_OFF,  "SID")                                    \
    E(LOG_SID_EXEC,     LOG_OFF,  "SID execution")                          \
    E(LOG_SIDREG,       LOG_OFF,  "SID registers")                          \
    E(LOG_AUD,          LOG_OFF,  "Sound generation")                       \
    E(LOG_AUDBUF,       LOG_OFF,  "Audio buffers")                          \
    E(LOG_AUDVOL,       LOG_OFF,  "Audio volume")                           \
                                                                              \
    /* Drive */                                                               \
    E(LOG_VIA,          LOG_OFF,  "VIA chip")                               \
    E(LOG_PIA,          LOG_OFF,  "PIA chip")                               \
    E(LOG_SER,          LOG_OFF,  "Serial port (IEC bus)")                  \
    E(LOG_DSK,          LOG_OFF,  "Disk")                                   \
    E(LOG_DSKCHG,       LOG_OFF,  "Disk change procedure")                  \
    E(LOG_GCR,          LOG_OFF,  "GCR encoding")                           \
    E(LOG_PAR,          LOG_OFF,  "Parallel port")                          \
                                                                              \
    /* Media */                                                               \
    E(LOG_IMG,          LOG_OFF,  "Disk images")                            \
    E(LOG_CRT,          LOG_OFF,  "Cartridges")                             \
    E(LOG_FILE,         LOG_OFF,  "Media files")                            \
                                                                              \
    /* Peripherals */                                                         \
    E(LOG_JOY,          LOG_OFF,  "Joystick")                               \
    E(LOG_DRV,          LOG_OFF,  "Floppy drive")                           \
    E(LOG_TAP,          LOG_OFF,  "Datasette")                              \
    E(LOG_KBD,          LOG_OFF,  "Keyboard")                               \
    E(LOG_PRT,          LOG_OFF,  "Ports")                                  \
    E(LOG_EXP,          LOG_OFF,  "Expansion port")                         \
    E(LOG_USR,          LOG_OFF,  "User port")                              \
                                                                              \
    /* Misc */                                                                \
    E(LOG_RSH,          LOG_OFF,  "RetroShell")                             \
    E(LOG_REC,          LOG_OFF,  "Screen recorder")                        \
    E(LOG_REU,          LOG_OFF,  "REU cartridge")                          \
    E(LOG_REU_DMA,      LOG_OFF,  "REU DMA transfers (very verbose)")       \
    E(LOG_SCK,          LOG_OFF,  "Sockets")                                \
    E(LOG_SRV,          LOG_OFF,  "Servers")                                \
    E(LOG_GDB,          LOG_OFF,  "GDB server")                             \
    E(LOG_DAP,          LOG_OFF,  "DAP server")


//
// Debug flags
//

#define VC_DEBUG_FLAGS(E)                                                     \
                                                                              \
    /* Run ahead */                                                           \
    E(RUA_CHECKSUM,     false, "Run-ahead instance integrity")                \
    E(RUA_ON_STEROIDS,  false, "Update RUA instance every frame")             \
                                                                              \
    /* Snapshots */                                                           \
    E(SNP_VERIFY,       false, "Verify snapshot integrity")                   \
                                                                              \
    /* VICII */                                                               \
    E(VICII_SAFE_MODE,  false, "Disable VICII fast-paths")                    \
    E(VICII_STATS,      false, "Collect VICII statistics")                    \
                                                                              \
    /* Drive */                                                               \
    E(FS_VERIFY,        false, "Verify file system integrity")                \
                                                                              \
    /* Forced error conditions */                                             \
    E(LAUNCH_ERROR,     false, "Force a launch error")                        \
    E(ROM_MISSING,      false, "Force a missing-KERNAL-ROM error")            \
    E(MEGA64_MISMATCH,  false, "Force a MEGA65 ROM version mismatch")         \
    E(SNAP_TOO_OLD,     false, "Force a 'snapshot too old' error")            \
    E(SNAP_TOO_NEW,     false, "Force a 'snapshot too new' error")            \
    E(SNAP_IS_BETA,     false, "Force a 'beta snapshot' error")               \
    E(SNAP_CORRUPTED,   false, "Force a snapshot corruption error")           \
    E(CRT_UNKNOWN,      false, "Force an 'unknown cartridge' error")          \
    E(CRT_UNSUPPORTED,  false, "Force an 'unsupported cartridge' error")      \
    E(RECORDING_ERROR,  false, "Force a screen-recording error")              \
    E(ZLIB_ERROR,       false, "Force a zlib decompression error")


//
// Logging macro and flag declarations
//

#define logme(key, format, ...) \
    do { \
        if CONSTEXPR (key != LOG_OFF) \
            log(key, std::source_location::current(), \
                format __VA_OPT__(,) __VA_ARGS__); \
    } while (0)

#define DECLARE_LOG_FLAG(name, dflt, help) \
    inline CONSTEXPR long name = dflt;
VC_LOG_FLAGS(DECLARE_LOG_FLAG)
#undef DECLARE_LOG_FLAG

#define DECLARE_DEBUG_FLAG(name, dflt, help) \
    inline CONSTEXPR bool name = dflt;
VC_DEBUG_FLAGS(DECLARE_DEBUG_FLAG)
#undef DECLARE_DEBUG_FLAG


//
// Flag descriptors (debug builds)
//

#ifndef NDEBUG

extern const std::vector<FlagInfo> logFlags;
extern const std::vector<FlagInfo> debugFlags;

#endif

}
