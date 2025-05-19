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

#include "OptionTypes.h"
#include "Reflection.h"
#include "Defaults.h"
#include <algorithm>

namespace vc64 {

typedef std::vector<Opt> Options;

class Configurable
{

public:

    virtual ~Configurable() = default;

    // Returns the available config options
    virtual const Options &getOptions() const = 0;

    // Returns true iff a specific option is available
    bool isValidOption(Opt opt) const;

    // Gets a config option
    virtual i64 getOption(Opt opt) const { return 0; }

    // Gets the fallback for a config option
    virtual i64 getFallback(Opt opt) const = 0;

    // Throws an exception of if the given option/value pair is invalid
    virtual void checkOption(Opt opt, i64 value) { }
    void checkOption(Opt opt, const string &value);
    void checkOption(const string &opt, const string &value);

    // Sets a config option
    virtual void setOption(Opt opt, i64 value) { }
    void setOption(Opt opt, const string &value);
    void setOption(const string &opt, const string &value);

    // Resets all config options
    void resetConfig(const Defaults &defaults, isize objid = 0);

    // Dumps the current configuration
    void dumpConfig(std::ostream& os) const;
};

}
