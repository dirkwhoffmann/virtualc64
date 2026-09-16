// -----------------------------------------------------------------------------
// This file is part of utlib - A lightweight utility library
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "utl/abilities/Wakeable.h"

namespace utl {

void
Wakeable::waitForWakeUp(utl::Time timeout)
{
    auto delay = std::chrono::nanoseconds(timeout.asNanoseconds());

    std::unique_lock<std::mutex> lock(condMutex);

    // Wait until we have at least one wakeup or timeout occurs
    condVar.wait_for(lock, delay, [this] {
        return wakeCount > 0;
    });

    // Consume one wakeup if available
    if (wakeCount > 0) {
        wakeCount--;
    }
}

void
Wakeable::wakeUp()
{
    {
        std::lock_guard<std::mutex> lock(condMutex);
        wakeCount++;  // accumulate wakeups (no loss)
    }

    condVar.notify_one();
}

} // namespace utl

/*
 
 // -----------------------------------------------------------------------------
// This file is part of utlib - A lightweight utility library
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "utl/abilities/Wakeable.h"

namespace utl {

void
Wakeable::waitForWakeUp(utl::Time timeout)
{
    auto now = std::chrono::system_clock::now();
    auto delay = std::chrono::nanoseconds(timeout.asNanoseconds());

    std::unique_lock<std::mutex> lock(condMutex);
    condVar.wait_until(lock, now + delay, [this]{ return ready; });
    ready = false;
}

void
Wakeable::wakeUp()
{
    {
        std::lock_guard<std::mutex> lock(condMutex);
        ready = true;
    }
    condVar.notify_one();
}

}
*/
