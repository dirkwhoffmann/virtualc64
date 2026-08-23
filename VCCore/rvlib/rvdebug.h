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
 *   builds); the library behaves identically either way. They are typed
 *   'LogLevel', because their value doubles as the severity the message is
 *   issued with. OFF disables the call, any other LogLevel enables it
 *   at that severity. All logging flags are prefixed 'LOG_'.
 *
 * - Debug flags enable extra behavior with a real side effect (an
 *   integrity check, a redundant computation compared against the fast
 *   path, forcing a specific code path, simulating an error condition,
 *   ...). They are all typed 'bool'.
 *
 * Both tables are X-macro lists: each entry names a flag exactly once, and
 * is expanded both into the variable declaration and (in debug builds) into
 * a descriptor table used by RetroShell. To add a flag, add one line here.
 */


//
// Logging flags
//

#define RV_LOG_FLAGS(E)                                                       \
                                                                              \
    /* File systems */                                                        \
    E(LOG_FS,         Off,  "File systems")                              \
                                                                              \
    /* Media */                                                               \
    E(LOG_IMG,        Off,  "Disk images")


//
// Debug flags
//

#define RV_DEBUG_FLAGS(E)                                                     \
                                                                              \
    /* File systems */                                                        \
    E(FS_VERIFY,      false, "Verify file system integrity")            \
                                                                              \
    /* Forced error conditions */                                             \
    E(HDR_TOO_LARGE,        false, "Force a 'drive too large' error")   \
    E(HDR_UNSUPPORTED_C,    false, "Force an unsupported 'C' geometry") \
    E(HDR_UNSUPPORTED_H,    false, "Force an unsupported 'H' geometry") \
    E(HDR_UNSUPPORTED_S,    false, "Force an unsupported 'S' geometry") \
    E(HDR_UNSUPPORTED_B,    false, "Force an unsupported 'B' geometry") \
    E(HDR_UNKNOWN_GEOMETRY, false, "Force an 'unknown geometry' error") \
    E(HDR_MODIFIED,         false, "Force the drive-modified flag")     \
    E(FS_WRONG_BSIZE,       false, "Force a wrong block-size error")    \
    E(FS_WRONG_CAPACITY,    false, "Force a wrong-capacity error")      \
    E(FS_WRONG_DOS_TYPE,    false, "Force a wrong DOS-type error")      \
    E(DMS_CANT_CREATE,      false, "Force a DMS-file creation error")


//
// Logging macro
//

#define logme(key, format, ...) \
    do { \
        if CONSTEXPR (key != utl::LogLevel::Off) \
            log(key, std::source_location::current(), \
                format __VA_OPT__(,) __VA_ARGS__); \
    } while (0)


namespace retro::vault {

using utl::LogLevel;
using utl::FlagInfo;


//
// Fixed severities (always active, never OFF)
//

inline constexpr LogLevel LV_OFF     = LogLevel::Off;
inline constexpr LogLevel LV_FATAL   = LogLevel::Fatal;
inline constexpr LogLevel LV_ERROR   = LogLevel::Error;
inline constexpr LogLevel LV_WARNING = LogLevel::Warn;
inline constexpr LogLevel LV_INFO    = LogLevel::Info;
inline constexpr LogLevel LV_DEBUG   = LogLevel::Debug;
inline constexpr LogLevel LV_TRACE   = LogLevel::Trace;


//
// Flag declarations (generated from the tables above)
//

#define DECLARE_LOG_FLAG(name, dflt, help) \
    inline CONSTEXPR LogLevel name = LogLevel::dflt;
RV_LOG_FLAGS(DECLARE_LOG_FLAG)
#undef DECLARE_LOG_FLAG

#define DECLARE_DEBUG_FLAG(name, dflt, help) \
    inline CONSTEXPR bool name = dflt;
RV_DEBUG_FLAGS(DECLARE_DEBUG_FLAG)
#undef DECLARE_DEBUG_FLAG


//
// Flag descriptors (debug builds only)
//

#ifndef NDEBUG

extern const std::vector<FlagInfo> logFlags;
extern const std::vector<FlagInfo> debugFlags;

#endif

}
