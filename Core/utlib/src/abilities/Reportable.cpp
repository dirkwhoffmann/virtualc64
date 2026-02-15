// -----------------------------------------------------------------------------
// This file is part of utlib - A lightweight utility library
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "utl/abilities/Reportable.h"
#include <ostream>
#include <iomanip>

namespace utl {

void
Reportable::report(std::ostream &os, isize category) const
{
    for (const auto& item : makeReport(category)) {

        os << std::setw(22) << std::left
        << item.key << ": "
        << item.value << "\n";
    }
}

}
