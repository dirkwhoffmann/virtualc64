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
    (void)future.get();
}

void
Wakeable::wakeUp()
{
    promise.set_value(true);
}

}
