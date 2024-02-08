// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// This FILE is dual-licensed. You are free to choose between:
//
//     - The GNU General Public License v3 (or any later version)
//     - The Mozilla Public License v2
//
// SPDX-License-Identifier: GPL-3.0-or-later OR MPL-2.0
// -----------------------------------------------------------------------------

#pragma once

#include "Concurrency.h"

namespace vc64 {

class Wakeable
{
    static constexpr auto timeout = std::chrono::milliseconds(100);

    std::mutex condMutex;
    std::condition_variable condVar;
    bool ready = false;

public:

    void waitForWakeUp(util::Time timeout);
    void wakeUp();
};

}
