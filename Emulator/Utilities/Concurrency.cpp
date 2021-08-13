// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "Concurrency.h"

namespace util {

void
Wakeable::waitForWakeUp()
{
#ifdef USE_CONDITION_VARIABLE
    
    std::unique_lock<std::mutex> lock(condMutex);
    condFlag = false;
    cond.wait_for(lock,
                  std::chrono::seconds(1000),
                  [this]() { return condFlag; } );

#else
    
    promise.set_value(true);

#endif
}

void
Wakeable::wakeUp()
{
#ifdef USE_CONDITION_VARIABLE
    
    {
        std::lock_guard<std::mutex> lock(condMutex);
        condFlag = true;
    }
    cond.notify_one();
    
#else
    
    (void)future.get();
    
#endif
}

}
