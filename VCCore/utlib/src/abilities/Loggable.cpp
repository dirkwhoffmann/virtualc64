// -----------------------------------------------------------------------------
// This file is part of utlib - A lightweight utility library
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "utl/abilities/Loggable.h"
#include <cstdarg>
#include <format>

namespace utl {

void
Loggable::log(LogLevel level,
              const std::source_location &loc,
              const char *fmt, ...) const
{
    fprintf(stderr, "%s", prefix(level, loc).c_str());

    switch (level) {

        case LogLevel::LOG_EMERG:   fprintf(stderr, "EMERGENCY: "); break;
        case LogLevel::LOG_CRIT:    fprintf(stderr, "CRITICAL: ");  break;
        case LogLevel::LOG_ERR:     fprintf(stderr, "ERROR: ");     break;
        case LogLevel::LOG_WARNING: fprintf(stderr, "WARNING: ");   break;
        case LogLevel::LOG_NOTICE:  fprintf(stderr, "NOTICE: ");    break;

        default:
            break;
    }

    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
}

string
Loggable::prefix(LogLevel level, const std::source_location &loc) const
{
    const isize verbosity = 2;

    const auto stem = std::filesystem::path(loc.file_name()).stem().string();

    switch (verbosity) {

        case 0:  return "";
        case 1:  return std::format("{}: ", stem);

        default:
            return std::format("{}.{}: ", stem, loc.line());
    }
}

}
