#pragma once

#include <source_location>

namespace retro::vault {

//
// Debug settings
//

// Default channels
constexpr long NULLDEV         = 0;
constexpr long STDERR          = 1;

// File systems
constexpr long FS_DEBUG        = 0;

// Media
constexpr long IMG_DEBUG       = 0;

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
// Logging channels
//

namespace utl::channel {

// Default channels
extern long NULLDEV;
extern long STDERR;

// File systems
extern long FS_DEBUG;

// Images
extern long IMG_DEBUG;

}


//
// Main logging macro
//

#if NDEBUG

#define logMsg(key, level, format, ...) \
    do { \
        if constexpr (key) \
            log(::retro::vault::channel::key, level, std::source_location::current(), \
                format __VA_OPT__(,) __VA_ARGS__); \
    } while (0)

#else

#define logMsg(key, level, format, ...)                           \
do { \
    log(::retro::vault::channel::key, level, std::source_location::current(), \
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

