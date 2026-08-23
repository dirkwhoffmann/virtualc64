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
 * - Action flags enable extra debug behavior with a real side effect
 *   (an integrity check, a redundant computation compared against the
 *   fast path, forcing a specific code path, ...). They are typed 'bool',
 *   except for those holding a parameter value.
 *
 * Both tables are X-macro lists: each entry names a flag exactly once, and
 * is expanded both into the variable declaration and (in debug builds) into
 * a descriptor table used by RetroShell. To add a flag, add one line here.
 */


//
// Logging flags
//

//        name          default    description
#define RV_LOG_FLAGS(E)                                                       \
                                                                              \
    /* File systems */                                                        \
    E(LOG_FS,         LV_OFF,  "File systems")                              \
                                                                              \
    /* Media */                                                               \
    E(LOG_IMG,        LV_OFF,  "Disk images")


//
// Action flags
//

//        type  name          default  description
#define RV_DEBUG_FLAGS(E)                                                     \
                                                                              \
    /* File systems */                                                        \
    E(bool, FS_VERIFY,      false, "Verify file system integrity")


//
// Logging macro
//

#define logme(key, format, ...) \
    do { \
        if CONSTEXPR (debug::key != utl::LogLevel::LV_OFF) \
            log(debug::key, std::source_location::current(), \
                format __VA_OPT__(,) __VA_ARGS__); \
    } while (0)


namespace retro::vault::debug {

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
RV_LOG_FLAGS(DECLARE_LOG_FLAG)
#undef DECLARE_LOG_FLAG

#define DECLARE_DEBUG_FLAG(type, name, dflt, help) \
    inline CONSTEXPR type name = dflt;
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


//
// Forced error conditions
//

namespace retro::vault::force {

constexpr long HDR_TOO_LARGE        = 0;
constexpr long HDR_UNSUPPORTED_C    = 0;
constexpr long HDR_UNSUPPORTED_H    = 0;
constexpr long HDR_UNSUPPORTED_S    = 0;
constexpr long HDR_UNSUPPORTED_B    = 0;
constexpr long HDR_UNKNOWN_GEOMETRY = 0;
constexpr long HDR_MODIFIED         = 0;
constexpr long FS_WRONG_BSIZE       = 0;
constexpr long FS_WRONG_CAPACITY    = 0;
constexpr long FS_WRONG_DOS_TYPE    = 0;
constexpr long DMS_CANT_CREATE      = 0;

}


//
// Convenience wrappers
//

#define fatal(format, ...) \
    do { \
        logme(LV_FATAL, format __VA_OPT__(,) __VA_ARGS__); \
        assert(false); \
        std::terminate(); \
    } while(0)
