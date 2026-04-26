// -----------------------------------------------------------------------------
// This file is part of utlib - A lightweight utility library
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "utl/chrono/Clock.h"

namespace utl {

Clock::Clock()
{
    start = Time::now();
}

void
Clock::updateElapsed()
{
    updateElapsed(Time::now());
}

void
Clock::updateElapsed(Time now)
{
    if (!paused) elapsed += now - start;
    start = now;
}

Time
Clock::getElapsedTime()
{
    updateElapsed();
    return elapsed;
}

Time
Clock::stop()
{
    updateElapsed();
    paused = true;
    return elapsed;
}

Time
Clock::go()
{
    updateElapsed();
    paused = false;
    return elapsed;
}

Time
Clock::restart()
{
    auto now = Time::now();

    updateElapsed(now);
    auto result = elapsed;

    start = now;
    elapsed = 0;
    paused = false;

    return result;
}

StopWatch::StopWatch(bool enable, const string &description) : enable(enable), description(description)
{
    if (enable) clock.restart();
}

StopWatch::~StopWatch()
{
    if (enable) {

        auto elapsed = clock.stop();
        fprintf(stderr, "%s %1.4f sec\n", description.c_str(), elapsed.asSeconds());
    }
}

}
