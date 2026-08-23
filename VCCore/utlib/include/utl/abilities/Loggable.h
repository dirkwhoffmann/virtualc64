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
 * stderr. Whether a call site actually calls log() at all is decided at
 * the call site itself (see the logging macros in debug.h), based on a
 * per-flag debug setting rather than a runtime channel lookup.
 */

#pragma once

#include "utl/abilities/Reflectable.h"
#include <source_location>

// Debug builds define DEBUG=1 as a preprocessor macro, which would otherwise
// clobber the LogLevel::DEBUG enumerator below.
#undef DEBUG

namespace utl {

enum class LogLevel : long
{
    LV_OFF   = 0,
    LV_FATAL = 1,
    LV_ERROR = 2,
    LV_WARN  = 3,
    LV_INFO  = 4,
    LV_DEBUG = 5,
    LV_TRACE = 6
};

struct LogLevelEnum : Reflectable<LogLevelEnum, LogLevel>
{
    static constexpr long minVal = 0;
    static constexpr long maxVal = (long)LogLevel::LV_TRACE;

    static const char *_key(long value) { return _key(LogLevel(value)); }
    static const char *_key(LogLevel value)
    {
        switch (value) {

            case LogLevel::LV_OFF:   return "OFF";
            case LogLevel::LV_FATAL: return "FATAL";
            case LogLevel::LV_ERROR: return "ERROR";
            case LogLevel::LV_WARN:  return "WARN";
            case LogLevel::LV_INFO:  return "INFO";
            case LogLevel::LV_DEBUG: return "DEBUG";
            case LogLevel::LV_TRACE: return "TRACE";
        }
        return "???";
    }
    static const char *help(long value) { return help(LogLevel(value)); }
    static const char *help(LogLevel value)
    {
        switch (value) {

            case LogLevel::LV_OFF:   return "Logging disabled";
            case LogLevel::LV_FATAL: return "Unrecoverable error";
            case LogLevel::LV_ERROR: return "Error condition";
            case LogLevel::LV_WARN:  return "Warning condition";
            case LogLevel::LV_INFO:  return "Informational message";
            case LogLevel::LV_DEBUG: return "Debug message";
            case LogLevel::LV_TRACE: return "Fine-grained trace message";
        }
        return "???";
    }
};

/* Descriptor of a single debug flag.
 *
 * Client code declares its flags in X-macro tables (see debug.h) and
 * expands those tables into a vector of descriptors. In projects that
 * combine several independent libraries, each with its own debug flags,
 * the descriptor gives RetroShell a uniform way to list and modify all of
 * them without any library having to know about the others. Both accessors
 * funnel through 'long', so that LogLevel, bool, and plain value flags can
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
    void log(LogLevel level,
             const std::source_location &loc,
             const char *fmt, ...) const;

    // Initializing
    Loggable() = default;
    virtual ~Loggable() = default;

protected:

    // Optional prefix printed prior to the debug message
    virtual string prefix(LogLevel, const std::source_location &) const;
};

}
