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
 *   'LogLevel', because their value doubles as the severity the message is
 *   issued with. OFF disables the call, any other LogLevel enables it
 *   at that severity.
 *
 * - Debug flags enable extra behavior with a real side effect (an
 *   integrity check, a redundant computation compared against the fast
 *   path, forcing a specific code path, simulating an error condition,
 *   ...). Some of these also log a message as part of that action, but
 *   disabling the flag changes what the emulator *does*, not just what
 *   it prints. They are typed 'bool', except for those holding a
 *   parameter value.
 *
 * Both tables are X-macro lists: each entry names a flag exactly once, and
 * is expanded both into the variable declaration and (in debug builds) into
 * a descriptor table used by RetroShell. To add a flag, add one line here.
 *
 * Note that rvlib maintains its own, independent set of flags (rvdebug.h).
 */


//
// Logging flags
//

#define VC_LOG_FLAGS(E)                                                       \
                                                                              \
    /* General */                                                             \
    E(LOG_XFILES,           LV_OFF,  "Report paranormal activity")            \
    E(LOG_CNF,              LV_OFF,  "Configuration options")                 \
    E(LOG_DEF,              LV_OFF,  "User defaults")                         \
                                                                              \
    /* Runloop */                                                             \
    E(LOG_RUN,              LV_OFF,  "Run loop, component states")            \
    E(LOG_TIM,              LV_OFF,  "Thread synchronization")                \
    E(LOG_WARP,             LV_OFF,  "Warp mode")                             \
    E(LOG_CMD,              LV_OFF,  "Command queue")                         \
    E(LOG_MSG,              LV_OFF,  "Message queue")                         \
    E(LOG_SNP,              LV_OFF,  "Serialization (snapshots)")             \
                                                                              \
    /* Run ahead */                                                           \
    E(LOG_RUA,              LV_OFF,  "Run-ahead activity")                    \
                                                                              \
    /* CPU */                                                                 \
    E(LOG_CPU,              LV_OFF,  "CPU")                                   \
    E(LOG_IRQ,              LV_OFF,  "CPU interrupts")                        \
                                                                              \
    /* Memory */                                                              \
    E(LOG_MEM,              LV_OFF,  "Memory")                                \
                                                                              \
    /* CIAs */                                                                \
    E(LOG_CIA,              LV_OFF,  "CIA execution")                         \
    E(LOG_CIAREG,           LV_OFF,  "CIA registers")                         \
                                                                              \
    /* VICII */                                                               \
    E(LOG_VICII,            LV_OFF,  "VICII execution")                       \
    E(LOG_VICII_REG,        LV_OFF,  "VICII registers")                       \
    E(LOG_RASTERIRQ,        LV_OFF,  "VICII raster IRQ")                      \
                                                                              \
    /* SID */                                                                 \
    E(LOG_SID,              LV_OFF,  "SID")                                   \
    E(LOG_SID_EXEC,         LV_OFF,  "SID execution")                         \
    E(LOG_SIDREG,           LV_OFF,  "SID registers")                         \
    E(LOG_AUD,              LV_OFF,  "Sound generation")                      \
    E(LOG_AUDBUF,           LV_OFF,  "Audio buffers")                         \
    E(LOG_AUDVOL,           LV_OFF,  "Audio volume")                          \
                                                                              \
    /* Drive */                                                               \
    E(LOG_VIA,              LV_OFF,  "VIA chip")                              \
    E(LOG_PIA,              LV_OFF,  "PIA chip")                              \
    E(LOG_SER,              LV_OFF,  "Serial port (IEC bus)")                 \
    E(LOG_DSK,              LV_OFF,  "Disk")                                  \
    E(LOG_DSKCHG,           LV_OFF,  "Disk change procedure")                 \
    E(LOG_GCR,              LV_OFF,  "GCR encoding")                          \
    E(LOG_PAR,              LV_OFF,  "Parallel port")                         \
                                                                              \
    /* Media */                                                               \
    E(LOG_IMG,              LV_OFF,  "Disk images")                           \
    E(LOG_CRT,              LV_OFF,  "Cartridges")                            \
    E(LOG_FILE,             LV_OFF,  "Media files")                           \
                                                                              \
    /* Peripherals */                                                         \
    E(LOG_JOY,              LV_OFF,  "Joystick")                              \
    E(LOG_DRV,              LV_OFF,  "Floppy drive")                          \
    E(LOG_TAP,              LV_OFF,  "Datasette")                             \
    E(LOG_KBD,              LV_OFF,  "Keyboard")                              \
    E(LOG_PRT,              LV_OFF,  "Ports")                                 \
    E(LOG_EXP,              LV_OFF,  "Expansion port")                        \
    E(LOG_USR,              LV_OFF,  "User port")                             \
                                                                              \
    /* Misc */                                                                \
    E(LOG_RSH,              LV_OFF,  "RetroShell")                            \
    E(LOG_REC,              LV_OFF,  "Screen recorder")                       \
    E(LOG_REU,              LV_OFF,  "REU cartridge")                         \
    E(LOG_REU_DMA,          LV_OFF,  "REU DMA transfers (very verbose)")      \
    E(LOG_SCK,              LV_OFF,  "Sockets")                               \
    E(LOG_SRV,              LV_OFF,  "Servers")                               \
    E(LOG_GDB,              LV_OFF,  "GDB server")                            \
    E(LOG_DAP,              LV_OFF,  "DAP server")


//
// Debug flags
//

#define VC_DEBUG_FLAGS(E)                                                     \
                                                                              \
    /* Run ahead */                                                           \
    E(bool, RUA_CHECKSUM,    false, "Run-ahead instance integrity")           \
    E(bool, RUA_ON_STEROIDS, false, "Update RUA instance every frame")        \
                                                                              \
    /* Snapshots */                                                           \
    E(bool, SNP_VERIFY,      false, "Verify snapshot integrity")              \
                                                                              \
    /* VICII */                                                               \
    E(bool, VICII_SAFE_MODE, false, "Disable VICII fast-paths")               \
    E(bool, VICII_STATS,     false, "Collect VICII statistics")               \
                                                                              \
    /* Drive */                                                               \
    E(bool, FS_VERIFY,       false, "Verify file system integrity")           \
                                                                              \
    /* Forced error conditions */                                             \
    E(bool, LAUNCH_ERROR,    false, "Force a launch error")                   \
    E(bool, ROM_MISSING,     false, "Force a missing-KERNAL-ROM error")       \
    E(bool, MEGA64_MISMATCH, false, "Force a MEGA65 ROM version mismatch")    \
    E(bool, SNAP_TOO_OLD,    false, "Force a 'snapshot too old' error")       \
    E(bool, SNAP_TOO_NEW,    false, "Force a 'snapshot too new' error")       \
    E(bool, SNAP_IS_BETA,    false, "Force a 'beta snapshot' error")          \
    E(bool, SNAP_CORRUPTED,  false, "Force a snapshot corruption error")      \
    E(bool, CRT_UNKNOWN,     false, "Force an 'unknown cartridge' error")     \
    E(bool, CRT_UNSUPPORTED, false, "Force an 'unsupported cartridge' error") \
    E(bool, RECORDING_ERROR, false, "Force a screen-recording error")         \
    E(bool, ZLIB_ERROR,      false, "Force a zlib decompression error")


//
// Logging macro
//

#define logme(key, format, ...) \
    do { \
        if CONSTEXPR (key != utl::LogLevel::LV_OFF) \
            log(key, std::source_location::current(), \
                format __VA_OPT__(,) __VA_ARGS__); \
    } while (0)


namespace vc64 {

using utl::LogLevel;
using utl::FlagInfo;


//
// Fixed severities (always active, never OFF)
//

inline constexpr LogLevel LV_OFF     = LogLevel::LV_OFF;
inline constexpr LogLevel LV_FATAL   = LogLevel::LV_FATAL;
inline constexpr LogLevel LV_ERROR   = LogLevel::LV_ERROR;
inline constexpr LogLevel LV_WARNING = LogLevel::LV_WARN;
inline constexpr LogLevel LV_INFO    = LogLevel::LV_INFO;
inline constexpr LogLevel LV_DEBUG   = LogLevel::LV_DEBUG;
inline constexpr LogLevel LV_TRACE   = LogLevel::LV_TRACE;


//
// Flag declarations (generated from the tables above)
//

#define DECLARE_LOG_FLAG(name, dflt, help) \
    inline CONSTEXPR LogLevel name = LogLevel::dflt;
VC_LOG_FLAGS(DECLARE_LOG_FLAG)
#undef DECLARE_LOG_FLAG

#define DECLARE_DEBUG_FLAG(type, name, dflt, help) \
    inline CONSTEXPR type name = dflt;
VC_DEBUG_FLAGS(DECLARE_DEBUG_FLAG)
#undef DECLARE_DEBUG_FLAG


//
// Flag descriptors (debug builds only)
//

#ifndef NDEBUG

extern const std::vector<FlagInfo> logFlags;
extern const std::vector<FlagInfo> debugFlags;

#endif

}
