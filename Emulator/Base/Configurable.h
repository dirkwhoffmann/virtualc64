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

#include "EmulatorTypes.h"
#include "Reflection.h"
#include <algorithm>

namespace vc64 {

typedef std::vector<std::pair<Option,string>> ConfigOptions;

class Configurable
{
    constexpr static ConfigOptions options = { };

    // Converters
    string opt2str(Option opt) const;
    Option str2opt(const string &opt) const;
    string arg2str(Option opt, i64 arg) const;
    i64 str2arg(Option opt, const string &arg) const;

public:

    virtual ~Configurable() { };

    // Returns all available config options
    virtual const ConfigOptions &getOptions() const { return options; }

    // Checks if a specific option is available
    bool isValidOption(Option opt) const;

    // Gets or sets a config option
    virtual i64 getOption(Option opt) const { return 0; }
    virtual void setOption(Option opt, i64 value) { }

    // Convinient wrappers
    i64 getOption(const string &opt) const { return getOption(str2opt(opt)); }
    void setOption(const string &opt, const string &value) { setOption(str2opt(opt), value); }
    void setOption(Option opt, const string &value) { setOption(opt, str2arg(opt, value)); }

    // Dumps the current configuration into a stream
    void dumpConfig(std::ostream& os) const;

    // Experimental
    string keyList() { return OptionEnum::keyList([&](long i) { return isValidOption(i); }); }
    string argList() { return OptionEnum::argList([&](long i) { return isValidOption(i); }); }
};

}
