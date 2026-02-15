// -----------------------------------------------------------------------------
// This file is part of utlib - A lightweight utility library
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "utl/common.h"

namespace utl {

struct ReportItem {

    string key;
    string value;
};

using Report = std::vector<ReportItem>;

class Reportable {

protected:

    virtual const Report makeReport(isize category = 0) const = 0;

public:

    Reportable() = default;
    virtual ~Reportable() = default;
    
    virtual void report(std::ostream &os, isize category = 0) const;
};

}
