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

std::vector<LogChannelInfo> &
Loggable::channels()
{
    static std::vector<LogChannelInfo> v;
    return v;
}

LogChannel
Loggable::subscribe(string name, optional<long> level, string description)
{
    if (level && *level >= LogLevelEnum::minVal && *level <= LogLevelEnum::maxVal) {

        return subscribe(std::move(name),
                  optional<LogLevel>(LogLevel(*level)),
                  std::move(description));
    } else {

        return subscribe(std::move(name),
                  optional<LogLevel>(std::nullopt),
                  std::move(description));
    }
}

LogChannel
Loggable::subscribe(string name, optional<LogLevel> level, string description)
{
    auto &chns = channels();

    // Reserve some space to speed up further registrations
    if (chns.empty()) chns.reserve(64);

    // Seek channel
    for (LogChannel i = 0; i < LogChannel(chns.size()); ++i)
        if (chns[i].name == name) return i;

    // It it does not exist, assign a new channel number
    const LogChannel id = LogChannel(chns.size());

    // Add a new channel
    chns.push_back(LogChannelInfo{

        .name        = std::move(name),
        .level       = level,
        .description = std::move(description)
    });

    return id;
}

void
Loggable::setLevel(isize nr, optional<LogLevel> level)
{
    if (nr < size())
        channels()[nr].level = level;;
}

void
Loggable::setLevel(string name, optional<LogLevel> level)
{
    for (auto&  c : channels())
        if (c.name == name) { c.level = level; return; }
}

void
Loggable::log(LogChannel c,
              LogLevel level,
              const std::source_location &loc,
              const char *fmt, ...) const
{
    auto &channel = channels().at(c);
    if (!channel.level || level > *channel.level) return;

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
