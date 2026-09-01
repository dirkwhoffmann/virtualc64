// -----------------------------------------------------------------------------
// This file is part of utlib - A lightweight utility library
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

/* The Loggable interface provides a framework for printing log messages.
 *
 * Messages are generated via the log function and are always written to
 * stderr.
 *
 * The log levels follow the conventional log4j-style hierarchy:
 *
 *   OFF:   The highest possible rank. Intended to turn off logging.
 *   FATAL: Severe errors that cause premature termination.
 *   ERROR: Other runtime errors or unexpected conditions.
 *   WARN:  Runtime situations that are undesirable or unexpected.
 *   INFO:  Interesting runtime events.
 *   DEBUG: Detailed information on the flow through the system.
 *   TRACE: Most detailed information.
 */

#pragma once

#include "utl/common.h"
#include <source_location>

namespace utl {

inline constexpr long LOG_OFF   = 0;
inline constexpr long LOG_FATAL = 1;
inline constexpr long LOG_ERROR = 2;
inline constexpr long LOG_WARN  = 3;
inline constexpr long LOG_INFO  = 4;
inline constexpr long LOG_DEBUG = 5;
inline constexpr long LOG_TRACE = 6;


/* Descriptor of a single debug flag.
 *
 * Client code declares its flags in X-macro tables (see debug.h) and
 * expands those tables into a vector of descriptors. In projects that
 * combine several independent libraries, each with its own debug flags,
 * the descriptor provides a uniform way to list and modify all of
 * them without any library having to know about the others. Both accessors
 * funnel through 'long', so that logging, bool, and plain value flags can
 * share a single descriptor type.
 *
 * Descriptor tables exist in debug builds only. In release builds the flags
 * are 'constexpr': they cannot be assigned, and taking their address would
 * needlessly emit all of them into the binary.
 */

struct FlagInfo {

    // Name of the flag, as written in the declaration table
    const char *name;

    // Human-readable description
    const char *help;

    // Indicates whether this flag is a boolean switch
    bool boolean;

    // Accessors
    long (*get)();
    void (*set)(long);
};

class Loggable {

public:

    // Output function (called by macro wrappers)
#if defined(__clang__)
    __attribute__((format(printf, 4, 5)))
#endif
    void log(long level,
             const std::source_location &loc,
             const char *fmt, ...) const;

    // Initializing
    Loggable() = default;
    virtual ~Loggable() = default;

protected:

    // Optional prefix printed prior to the debug message
    virtual string prefix(long, const std::source_location &) const;
};


//
// Logging macros
//

#ifdef logmsg
#undef logmsg
#endif

#define logmsg(key, format, ...) \
    do { \
        if CONSTEXPR (key != LOG_OFF) \
            log(key, std::source_location::current(), \
                format __VA_OPT__(,) __VA_ARGS__); \
    } while (0)

#define xfiles(format, ...) \
    logmsg(LOG_XFILES, format __VA_OPT__(,) __VA_ARGS__)

}
