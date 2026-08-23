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
 * - Logging flags gate a logmsg() call and nothing else. Disabling one
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

namespace retro::vault {

using utl::LOG_OFF;
using utl::LOG_FATAL;
using utl::LOG_ERROR;
using utl::LOG_WARN;
using utl::LOG_INFO;
using utl::LOG_DEBUG;
using utl::LOG_TRACE;
using utl::FlagInfo;


//
// Logging flags
//

#define RV_LOG_FLAGS(E)                                                       \
                                                                              \
    /* File systems */                                                        \
    E(LOG_FS,               LOG_OFF,  "File systems")                         \
                                                                              \
    /* Media */                                                               \
    E(LOG_IMG,              LOG_OFF,  "Disk images")


//
// Debug flags
//

#define RV_DEBUG_FLAGS(E)                                                     \
                                                                              \
    /* File systems */                                                        \
    E(FS_VERIFY,            false, "Verify file system integrity")            \
                                                                              \
    /* Forced error conditions */                                             \
    E(HDR_TOO_LARGE,        false, "Force a 'drive too large' error")         \
    E(HDR_UNSUPPORTED_C,    false, "Force an unsupported 'C' geometry")       \
    E(HDR_UNSUPPORTED_H,    false, "Force an unsupported 'H' geometry")       \
    E(HDR_UNSUPPORTED_S,    false, "Force an unsupported 'S' geometry")       \
    E(HDR_UNSUPPORTED_B,    false, "Force an unsupported 'B' geometry")       \
    E(HDR_UNKNOWN_GEOMETRY, false, "Force an 'unknown geometry' error")       \
    E(HDR_MODIFIED,         false, "Force the drive-modified flag")           \
    E(FS_WRONG_BSIZE,       false, "Force a wrong block-size error")          \
    E(FS_WRONG_CAPACITY,    false, "Force a wrong-capacity error")            \
    E(FS_WRONG_DOS_TYPE,    false, "Force a wrong DOS-type error")            \
    E(DMS_CANT_CREATE,      false, "Force a DMS-file creation error")


//
// Flag declarations
//

#define DECLARE_LOG_FLAG(name, dflt, help) \
    inline CONSTEXPR long name = dflt;
RV_LOG_FLAGS(DECLARE_LOG_FLAG)
#undef DECLARE_LOG_FLAG

#define DECLARE_DEBUG_FLAG(name, dflt, help) \
    inline CONSTEXPR bool name = dflt;
RV_DEBUG_FLAGS(DECLARE_DEBUG_FLAG)
#undef DECLARE_DEBUG_FLAG


//
// Flag descriptors (debug builds)
//

#ifndef NDEBUG

extern const std::vector<FlagInfo> logFlags;
extern const std::vector<FlagInfo> debugFlags;

#endif

}
