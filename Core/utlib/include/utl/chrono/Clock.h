// -----------------------------------------------------------------------------
// This file is part of utlib - A lightweight utility library
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "utl/chrono/Time.h"

namespace utl {

class Clock {

    Time start;
    Time elapsed;

    bool paused = false;

    void updateElapsed();
    void updateElapsed(Time now);

public:

    Clock();

    Time getElapsedTime();

    Time stop();
    Time go();
    Time restart();
};

class StopWatch {

    bool enable;
    string description;
    Clock clock;

public:

    StopWatch(bool enable, const string &description);
    StopWatch(const string &description = "") : StopWatch(true, description) { }
    ~StopWatch();
};

}
