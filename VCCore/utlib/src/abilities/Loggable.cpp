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
Loggable::log(long level,
              const std::source_location &loc,
              const char *fmt, ...) const
{
    fprintf(stderr, "%s", prefix(level, loc).c_str());

    // Severities, per each client project's LOG_OFF..LOG_TRACE scheme
    switch (level) {

        case 1: fprintf(stderr, "FATAL: "); break;
        case 2: fprintf(stderr, "ERROR: "); break;
        case 3: fprintf(stderr, "WARN: ");  break;

        default:
            break;
    }

    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
}

string
Loggable::prefix(long level, const std::source_location &loc) const
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
