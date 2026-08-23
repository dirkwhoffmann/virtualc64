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

namespace utl {

/* Logging severities. Levels are plain 'long' values rather than an enum, so
 * that a logging flag and an ordinary value flag can share the same
 * declaration and descriptor machinery (see FlagInfo below). The symbolic
 * names (OFF, FATAL, ...) are the ones a client project declares for itself,
 * typically as 'LOG_OFF', 'LOG_FATAL', etc.; this table only needs to know
 * their numeric meaning to print and parse them.
 */

struct LogLevelEnum : Reflectable<LogLevelEnum, long>
{
    static constexpr long minVal = 0;
    static constexpr long maxVal = 6;

    static const char *_key(long value)
    {
        switch (value) {

            case 0: return "OFF";
            case 1: return "FATAL";
            case 2: return "ERROR";
            case 3: return "WARN";
            case 4: return "INFO";
            case 5: return "DEBUG";
            case 6: return "TRACE";
        }
        return "???";
    }
    static const char *help(long value)
    {
        switch (value) {

            case 0: return "Logging disabled";
            case 1: return "Unrecoverable error";
            case 2: return "Error condition";
            case 3: return "Warning condition";
            case 4: return "Informational message";
            case 5: return "Debug message";
            case 6: return "Fine-grained trace message";
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

}
