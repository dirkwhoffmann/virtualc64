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
#include "Dumpable.h"

namespace vc64 {

void
Dumpable::dump(Category category, std::ostream &ss) const
{
    _dump(category, ss);
}

void
Dumpable::dump(Category category) const
{
    dump(category, std::cout);
}

}
