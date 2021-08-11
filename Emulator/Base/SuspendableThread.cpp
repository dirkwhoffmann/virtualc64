// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "SuspendableThread.h"

void
SuspendableThread::suspend()
{
    debug(RUN_DEBUG, "Suspending (%zu)...\n", suspendCounter);
    
    if (suspendCounter || isRunning()) {
        pause();
        suspendCounter++;
    }
}

void
SuspendableThread::resume()
{
    debug(RUN_DEBUG, "Resuming (%zu)...\n", suspendCounter);
    
    if (suspendCounter && --suspendCounter == 0) {
        run();
    }
}

