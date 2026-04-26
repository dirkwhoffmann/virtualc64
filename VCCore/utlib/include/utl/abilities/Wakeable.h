// -----------------------------------------------------------------------------
// This file is part of utlib - A lightweight utility library
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "utl/chrono.h"
#include <condition_variable>

namespace utl {

class Wakeable
{
    static constexpr auto timeout = std::chrono::milliseconds(100);

    std::mutex condMutex;
    std::condition_variable condVar;
    bool ready = false;

public:

    void waitForWakeUp(utl::Time timeout);
    void wakeUp();
};

}
