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

#include "config.h"
#include "CoreObject.h"
#include <iostream>

namespace vc64 {

isize
CoreObject::verbosity = 2;

void
CoreObject::prefix(isize level, const char *component, isize line) const
{
    if (level == 1) {
        fprintf(stderr, "%s: ", objectName());
    }
    if (level >= 2) {
        fprintf(stderr, "%s:%ld ", objectName(), line);
    }
}

}
