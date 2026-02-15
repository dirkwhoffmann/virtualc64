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

#include <source_location>

namespace utl::debug {

//
// Debug settings
//

// Default channels
constexpr long NULLDEV         = 0;
constexpr long STDERR          = 1;

// General
constexpr long XFILES          = 0;
constexpr long CNF_DEBUG       = 0;
constexpr long DEF_DEBUG       = 0;

// Runloop
constexpr long RUN_DEBUG       = 0;
constexpr long TIM_DEBUG       = 0;
constexpr long WARP_DEBUG      = 0;
constexpr long CMD_DEBUG       = 0;
constexpr long MSG_DEBUG       = 0;
constexpr long SNP_DEBUG       = 0;

// Run ahead
constexpr long RUA_DEBUG       = 0;
constexpr long RUA_CHECKSUM    = 0;
constexpr long RUA_ON_STEROIDS = 0;

// CPU
constexpr long CPU_DEBUG       = 0;
constexpr long IRQ_DEBUG       = 0;

// Memory
constexpr long MEM_DEBUG       = 0;

// CIAs
constexpr long CIA_DEBUG       = 0;
constexpr long CIAREG_DEBUG    = 0;

// VICII
constexpr long VICII_DEBUG     = 0;
constexpr long VICII_REG_DEBUG = 0;
constexpr long VICII_SAFE_MODE = 0;
constexpr long VICII_STATS     = 0;
constexpr long RASTERIRQ_DEBUG = 0;

// SID
constexpr long SID_DEBUG       = 0;
constexpr long SID_EXEC        = 0;
constexpr long SIDREG_DEBUG    = 0;
constexpr long AUD_DEBUG       = 0;
constexpr long AUDBUF_DEBUG    = 0;
constexpr long AUDVOL_DEBUG    = 0;

// Drive
constexpr long VIA_DEBUG       = 0;
constexpr long PIA_DEBUG       = 0;
constexpr long SER_DEBUG       = 0;
constexpr long DSK_DEBUG       = 0;
constexpr long DSKCHG_DEBUG    = 0;
constexpr long GCR_DEBUG       = 0;
constexpr long FS_DEBUG        = 0;
constexpr long PAR_DEBUG       = 0;

// Media
constexpr long CRT_DEBUG       = 0;
constexpr long FILE_DEBUG      = 0;

// Peripherals
constexpr long JOY_DEBUG       = 0;
constexpr long DRV_DEBUG       = 0;
constexpr long TAP_DEBUG       = 0;
constexpr long KBD_DEBUG       = 0;
constexpr long PRT_DEBUG       = 0;
constexpr long EXP_DEBUG       = 0;
constexpr long USR_DEBUG       = 0;

// Other components
constexpr long RSH_DEBUG       = 0;
constexpr long REC_DEBUG       = 0;
constexpr long REU_DEBUG       = 0;
constexpr long SCK_DEBUG       = 0;
constexpr long SRV_DEBUG       = 1;
constexpr long GDB_DEBUG       = 0;
constexpr long DAP_DEBUG       = 0;

}

//
// Forced error conditions
//

namespace utl::force {

constexpr long LAUNCH_ERROR     = 0;
constexpr long ROM_MISSING      = 0;
constexpr long MEGA64_MISMATCH  = 0;
constexpr long SNAP_TOO_OLD     = 0;
constexpr long SNAP_TOO_NEW     = 0;
constexpr long SNAP_IS_BETA     = 0;
constexpr long SNAP_CORRUPTED   = 0;
constexpr long CRT_UNKNOWN      = 0;
constexpr long CRT_UNSUPPORTED  = 0;
constexpr long RECORDING_ERROR  = 0;
constexpr long ZLIB_ERROR       = 0;

}

//
// Logging channels
//

/* For each debug setting, the emulator registers a corresponding log channel
 * with the Loggable interface. Log channels initialized with a debug level
 * of 0 are disabled by default.
 *
 * In debug builds, log channels can be enabled, disabled, or reconfigured
 * dynamically via the Loggable interface. In release builds, the initial
 * configuration is fixed and cannot be modified, allowing the compiler
 * to apply performance optimization.
 */

namespace utl::channel {

// Default channels
extern long NULLDEV;
extern long STDERR;

// General
extern long XFILES;
extern long CNF_DEBUG;
extern long DEF_DEBUG;

// Runloop
extern long RUN_DEBUG;
extern long TIM_DEBUG;
extern long WARP_DEBUG;
extern long CMD_DEBUG;
extern long MSG_DEBUG;
extern long SNP_DEBUG;

// Run ahead
extern long RUA_DEBUG;
extern long RUA_CHECKSUM;
extern long RUA_ON_STEROIDS;

// CPU
extern long CPU_DEBUG;
extern long IRQ_DEBUG;

// Memory
extern long MEM_DEBUG;

// CIAs
extern long CIA_DEBUG;
extern long CIAREG_DEBUG;

// VICII
extern long VICII_DEBUG;
extern long VICII_REG_DEBUG;
extern long VICII_SAFE_MODE;
extern long VICII_STATS;
extern long RASTERIRQ_DEBUG;

// SID
extern long SID_DEBUG;
extern long SID_EXEC;
extern long SIDREG_DEBUG;
extern long AUD_DEBUG;
extern long AUDBUF_DEBUG;
extern long AUDVOL_DEBUG;

// Drive
extern long VIA_DEBUG;
extern long PIA_DEBUG;
extern long SER_DEBUG;
extern long DSK_DEBUG;
extern long DSKCHG_DEBUG;
extern long GCR_DEBUG;
extern long FS_DEBUG;
extern long PAR_DEBUG;

// Media
extern long CRT_DEBUG;
extern long FILE_DEBUG;

// Peripherals
extern long JOY_DEBUG;
extern long DRV_DEBUG;
extern long TAP_DEBUG;
extern long KBD_DEBUG;
extern long PRT_DEBUG;
extern long EXP_DEBUG;
extern long USR_DEBUG;

// Other components
extern long RSH_DEBUG;
extern long REC_DEBUG;
extern long REU_DEBUG;
extern long SCK_DEBUG;
extern long SRV_DEBUG;
extern long GDB_DEBUG;
extern long DAP_DEBUG;

}


//
// Main logging macro
//

#if NDEBUG

#define logMsg(key, level, format, ...) \
    do { \
        if constexpr (debug::key) \
            log(channel::key, level, std::source_location::current(), \
                format __VA_OPT__(,) __VA_ARGS__); \
    } while (0)

#else

#define logMsg(key, level, format, ...)                           \
do { \
    log(channel::key, level, std::source_location::current(), \
        format __VA_OPT__(,) __VA_ARGS__); \
} while (0)

#endif


//
// Wrappers for all syslog levels
//

#define logemergency(format, ...) \
    logMsg(STDERR, LogLevel::LOG_EMERG, format __VA_OPT__(,) __VA_ARGS__)

#define logalert(format, ...) \
    logMsg(STDERR, LogLevel::LOG_ALERT, format __VA_OPT__(,) __VA_ARGS__)

#define logcritical(format, ...) \
    logMsg(STDERR, LogLevel::LOG_CRIT, format __VA_OPT__(,) __VA_ARGS__)

#define logerror(format, ...) \
    logMsg(STDERR, LogLevel::LOG_ERR, format __VA_OPT__(,) __VA_ARGS__)

#define logwarn(format, ...) \
    logMsg(STDERR, LogLevel::LOG_WARNING, format __VA_OPT__(,) __VA_ARGS__)

#define lognotice(channel, format, ...) \
    logMsg(channel, LogLevel::LOG_NOTICE, format __VA_OPT__(,) __VA_ARGS__)

#define loginfo(channel, format, ...) \
    logMsg(channel, LogLevel::LOG_INFO, format __VA_OPT__(,) __VA_ARGS__)

#define logdebug(channel, format, ...) \
    logMsg(channel, LogLevel::LOG_DEBUG, format __VA_OPT__(,) __VA_ARGS__)

#define lognull(channel, format, ...)


//
// Convenience wrappers
//

#define fatal(format, ...) \
    do { \
        logemergency(format __VA_OPT__(,) __VA_ARGS__); \
        assert(false); \
        std::terminate(); \
    } while(0)

#define xfiles(format, ...) \
    logMsg(XFILES, LogLevel::LOG_INFO, format __VA_OPT__(,) __VA_ARGS__)
