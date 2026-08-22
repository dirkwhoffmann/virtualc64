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

/* In release builds (NDEBUG), CONSTEXPR expands to 'constexpr'. Every debug
 * flag below then becomes a compile-time constant, and 'if CONSTEXPR' below
 * becomes 'if constexpr', so the compiler removes each guarded log call (or
 * action) entirely when its flag is off.
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
 *   issued with. LOG_NONE disables the call, any other LogLevel enables it
 *   at that severity. All logging flags are prefixed 'LOG_'.
 *
 * - Action flags enable extra debug behavior with a real side effect
 *   (an integrity check, a redundant computation compared against the
 *   fast path, forcing a specific code path, ...). Some of these also log
 *   a message as part of that action, but disabling the flag changes what
 *   the emulator *does*, not just what it prints. They are typed 'bool',
 *   except for those holding a parameter value.
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

//        name            default    description
#define VC_LOG_FLAGS(E)                                                       \
                                                                              \
    /* General */                                                             \
    E(LOG_XFILES,       LOG_NONE,  "Report paranormal activity")              \
    E(LOG_CNF,          LOG_NONE,  "Configuration options")                   \
    E(LOG_DEF,          LOG_NONE,  "User defaults")                           \
                                                                              \
    /* Runloop */                                                             \
    E(LOG_RUN,          LOG_NONE,  "Run loop, component states")              \
    E(LOG_TIM,          LOG_NONE,  "Thread synchronization")                  \
    E(LOG_WARP,         LOG_NONE,  "Warp mode")                               \
    E(LOG_CMD,          LOG_NONE,  "Command queue")                           \
    E(LOG_MSG,          LOG_NONE,  "Message queue")                           \
    E(LOG_SNP,          LOG_NONE,  "Serialization (snapshots)")               \
                                                                              \
    /* Run ahead */                                                           \
    E(LOG_RUA,          LOG_NONE,  "Run-ahead activity")                      \
                                                                              \
    /* CPU */                                                                 \
    E(LOG_CPU,          LOG_NONE,  "CPU")                                     \
    E(LOG_IRQ,          LOG_NONE,  "CPU interrupts")                          \
                                                                              \
    /* Memory */                                                              \
    E(LOG_MEM,          LOG_NONE,  "Memory")                                  \
                                                                              \
    /* CIAs */                                                                \
    E(LOG_CIA,          LOG_NONE,  "CIA execution")                           \
    E(LOG_CIAREG,       LOG_NONE,  "CIA registers")                           \
                                                                              \
    /* VICII */                                                               \
    E(LOG_VICII,        LOG_NONE,  "VICII execution")                         \
    E(LOG_VICII_REG,    LOG_NONE,  "VICII registers")                         \
    E(LOG_RASTERIRQ,    LOG_NONE,  "VICII raster IRQ")                        \
                                                                              \
    /* SID */                                                                 \
    E(LOG_SID,          LOG_NONE,  "SID")                                     \
    E(LOG_SID_EXEC,     LOG_NONE,  "SID execution")                           \
    E(LOG_SIDREG,       LOG_NONE,  "SID registers")                           \
    E(LOG_AUD,          LOG_NONE,  "Sound generation")                        \
    E(LOG_AUDBUF,       LOG_NONE,  "Audio buffers")                           \
    E(LOG_AUDVOL,       LOG_NONE,  "Audio volume")                            \
                                                                              \
    /* Drive */                                                               \
    E(LOG_VIA,          LOG_NONE,  "VIA chip")                                \
    E(LOG_PIA,          LOG_NONE,  "PIA chip")                                \
    E(LOG_SER,          LOG_NONE,  "Serial port (IEC bus)")                   \
    E(LOG_DSK,          LOG_NONE,  "Disk")                                    \
    E(LOG_DSKCHG,       LOG_NONE,  "Disk change procedure")                   \
    E(LOG_GCR,          LOG_NONE,  "GCR encoding")                            \
    E(LOG_PAR,          LOG_NONE,  "Parallel port")                           \
                                                                              \
    /* Media */                                                               \
    E(LOG_IMG,          LOG_NONE,  "Disk images")                             \
    E(LOG_CRT,          LOG_NONE,  "Cartridges")                              \
    E(LOG_FILE,         LOG_NONE,  "Media files")                             \
                                                                              \
    /* Peripherals */                                                         \
    E(LOG_JOY,          LOG_NONE,  "Joystick")                                \
    E(LOG_DRV,          LOG_NONE,  "Floppy drive")                            \
    E(LOG_TAP,          LOG_NONE,  "Datasette")                               \
    E(LOG_KBD,          LOG_NONE,  "Keyboard")                                \
    E(LOG_PRT,          LOG_NONE,  "Ports")                                   \
    E(LOG_EXP,          LOG_NONE,  "Expansion port")                          \
    E(LOG_USR,          LOG_NONE,  "User port")                               \
                                                                              \
    /* Misc */                                                                \
    E(LOG_RSH,          LOG_NONE,  "RetroShell")                              \
    E(LOG_REC,          LOG_NONE,  "Screen recorder")                         \
    E(LOG_REU,          LOG_NONE,  "REU cartridge")                           \
    E(LOG_REU_DMA,      LOG_NONE,  "REU DMA transfers (very verbose)")        \
    E(LOG_SCK,          LOG_NONE,  "Sockets")                                 \
    E(LOG_SRV,          LOG_NONE,  "Servers")                                 \
    E(LOG_GDB,          LOG_NONE,  "GDB server")                              \
    E(LOG_DAP,          LOG_NONE,  "DAP server")


//
// Action flags
//

//        type  name             default  description
#define VC_DEBUG_FLAGS(E)                                                     \
                                                                              \
    /* Run ahead */                                                           \
    E(bool, RUA_CHECKSUM,      false, "Run-ahead instance integrity")         \
    E(bool, RUA_ON_STEROIDS,   false, "Update RUA instance every frame")      \
                                                                              \
    /* Snapshots */                                                           \
    E(bool, SNP_VERIFY,        false, "Verify snapshot integrity")            \
                                                                              \
    /* VICII */                                                               \
    E(long, VICII_SAFE_MODE,   0,     "Disable VICII fast-paths")             \
    E(bool, VICII_STATS,       false, "Collect VICII statistics")             \
                                                                              \
    /* Drive */                                                               \
    E(bool, FS_VERIFY,         false, "Verify file system integrity")


//
// Logging macro
//

#define logme(key, format, ...) \
    do { \
        if CONSTEXPR (debug::key != utl::LogLevel::LOG_NONE) \
            log(debug::key, std::source_location::current(), \
                format __VA_OPT__(,) __VA_ARGS__); \
    } while (0)


namespace vc64::debug {

using utl::LogLevel;
using utl::FlagInfo;

//
// Fixed severities (always active, never LOG_NONE)
//

inline constexpr LogLevel LV_EMERGENCY = LogLevel::LOG_EMERG;
inline constexpr LogLevel LV_ALERT     = LogLevel::LOG_ALERT;
inline constexpr LogLevel LV_CRITICAL  = LogLevel::LOG_CRIT;
inline constexpr LogLevel LV_ERROR     = LogLevel::LOG_ERR;
inline constexpr LogLevel LV_WARNING   = LogLevel::LOG_WARNING;
inline constexpr LogLevel LV_NOTICE    = LogLevel::LOG_NOTICE;
inline constexpr LogLevel LV_INFO      = LogLevel::LOG_INFO;
inline constexpr LogLevel LV_DEBUG     = LogLevel::LOG_DEBUG;

// Always-off placeholder, used to permanently silence a log call
inline constexpr LogLevel LOG_NULLDEV = LogLevel::LOG_NONE;


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

//
// Forced error conditions
//

namespace vc64::force {

constexpr long LAUNCH_ERROR         = 0;
constexpr long ROM_MISSING          = 0;
constexpr long MEGA64_MISMATCH      = 0;
constexpr long SNAP_TOO_OLD         = 0;
constexpr long SNAP_TOO_NEW         = 0;
constexpr long SNAP_IS_BETA         = 0;
constexpr long SNAP_CORRUPTED       = 0;
constexpr long CRT_UNKNOWN          = 0;
constexpr long CRT_UNSUPPORTED      = 0;
constexpr long RECORDING_ERROR      = 0;
constexpr long ZLIB_ERROR           = 0;

}
