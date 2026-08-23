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
 *   it prints. They are all typed 'bool'.
 *
 * Both tables are X-macro lists: each entry names a flag exactly once, and
 * is expanded both into the variable declaration and (in debug builds) into
 * a descriptor table used by RetroShell. To add a flag, add one line here.
 *
 * Note that rvlib maintains its own, independent set of flags (rvdebug.h).
 */

namespace vc64 {

using utl::LogLevel;
using utl::FlagInfo;

inline constexpr LogLevel LV_OFF     = LogLevel::Off;
inline constexpr LogLevel LV_FATAL   = LogLevel::Fatal;
inline constexpr LogLevel LV_ERROR   = LogLevel::Error;
inline constexpr LogLevel LV_WARNING = LogLevel::Warn;
inline constexpr LogLevel LV_INFO    = LogLevel::Info;
inline constexpr LogLevel LV_DEBUG   = LogLevel::Debug;
inline constexpr LogLevel LV_TRACE   = LogLevel::Trace;


//
// Logging flags
//

#define VC_LOG_FLAGS(E)                                                       \
                                                                              \
    /* General */                                                             \
    E(LOG_XFILES,       Off,  "Report paranormal activity")                \
    E(LOG_CNF,          Off,  "Configuration options")                     \
    E(LOG_DEF,          Off,  "User defaults")                             \
                                                                              \
    /* Runloop */                                                             \
    E(LOG_RUN,          Off,  "Run loop, component states")                \
    E(LOG_TIM,          Off,  "Thread synchronization")                    \
    E(LOG_WARP,         Off,  "Warp mode")                                 \
    E(LOG_CMD,          Off,  "Command queue")                             \
    E(LOG_MSG,          Off,  "Message queue")                             \
    E(LOG_SNP,          Off,  "Serialization (snapshots)")                 \
                                                                              \
    /* Run ahead */                                                           \
    E(LOG_RUA,          Off,  "Run-ahead activity")                        \
                                                                              \
    /* CPU */                                                                 \
    E(LOG_CPU,          Off,  "CPU")                                       \
    E(LOG_IRQ,          Off,  "CPU interrupts")                            \
                                                                              \
    /* Memory */                                                              \
    E(LOG_MEM,          Off,  "Memory")                                    \
                                                                              \
    /* CIAs */                                                                \
    E(LOG_CIA,          Off,  "CIA execution")                             \
    E(LOG_CIAREG,       Off,  "CIA registers")                             \
                                                                              \
    /* VICII */                                                               \
    E(LOG_VICII,        Off,  "VICII execution")                           \
    E(LOG_VICII_REG,    Off,  "VICII registers")                           \
    E(LOG_RASTERIRQ,    Off,  "VICII raster IRQ")                          \
                                                                              \
    /* SID */                                                                 \
    E(LOG_SID,          Off,  "SID")                                       \
    E(LOG_SID_EXEC,     Off,  "SID execution")                             \
    E(LOG_SIDREG,       Off,  "SID registers")                             \
    E(LOG_AUD,          Off,  "Sound generation")                          \
    E(LOG_AUDBUF,       Off,  "Audio buffers")                             \
    E(LOG_AUDVOL,       Off,  "Audio volume")                              \
                                                                              \
    /* Drive */                                                               \
    E(LOG_VIA,          Off,  "VIA chip")                                  \
    E(LOG_PIA,          Off,  "PIA chip")                                  \
    E(LOG_SER,          Off,  "Serial port (IEC bus)")                     \
    E(LOG_DSK,          Off,  "Disk")                                      \
    E(LOG_DSKCHG,       Off,  "Disk change procedure")                     \
    E(LOG_GCR,          Off,  "GCR encoding")                              \
    E(LOG_PAR,          Off,  "Parallel port")                             \
                                                                              \
    /* Media */                                                               \
    E(LOG_IMG,          Off,  "Disk images")                               \
    E(LOG_CRT,          Off,  "Cartridges")                                \
    E(LOG_FILE,         Off,  "Media files")                               \
                                                                              \
    /* Peripherals */                                                         \
    E(LOG_JOY,          Off,  "Joystick")                                  \
    E(LOG_DRV,          Off,  "Floppy drive")                              \
    E(LOG_TAP,          Off,  "Datasette")                                 \
    E(LOG_KBD,          Off,  "Keyboard")                                  \
    E(LOG_PRT,          Off,  "Ports")                                     \
    E(LOG_EXP,          Off,  "Expansion port")                            \
    E(LOG_USR,          Off,  "User port")                                 \
                                                                              \
    /* Misc */                                                                \
    E(LOG_RSH,          Off,  "RetroShell")                                \
    E(LOG_REC,          Off,  "Screen recorder")                           \
    E(LOG_REU,          Off,  "REU cartridge")                             \
    E(LOG_REU_DMA,      Off,  "REU DMA transfers (very verbose)")          \
    E(LOG_SCK,          Off,  "Sockets")                                   \
    E(LOG_SRV,          Off,  "Servers")                                   \
    E(LOG_GDB,          Off,  "GDB server")                                \
    E(LOG_DAP,          Off,  "DAP server")


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
        if CONSTEXPR (key != utl::LogLevel::Off) \
            log(key, std::source_location::current(), \
                format __VA_OPT__(,) __VA_ARGS__); \
    } while (0)

#define DECLARE_LOG_FLAG(name, dflt, help) \
    inline CONSTEXPR LogLevel name = LogLevel::dflt;
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
