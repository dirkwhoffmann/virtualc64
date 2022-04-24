// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "C64Object.h"
#include <iostream>

bool
C64Object::verbose = true;

void
C64Object::prefix() const
{
    fprintf(stderr, "%s: ", getDescription());
}

void
C64Object::dump(dump::Category category, std::ostream& ss) const
{
    _dump(category, ss);
}

void
C64Object::dump(dump::Category category) const
{
    dump(category, std::cout);
}

void
C64Object::dump(std::ostream& ss) const
{
    dump((dump::Category)(-1), ss);
}

void
C64Object::dump() const
{
    dump((dump::Category)(-1));
}
