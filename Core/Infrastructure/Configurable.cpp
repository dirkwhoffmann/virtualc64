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

#include "VirtualC64Config.h"
#include "Configurable.h"
#include "Option.h"
#include "Defaults.h"
#include "IOUtils.h"
#include <algorithm>

namespace vc64 {

using namespace util;

bool
Configurable::isValidOption(Opt opt) const
{
    for (auto &it: getOptions()) {
        if (it == opt) return true;
    }
    return false;
}

void
Configurable::checkOption(Opt opt, const string &value)
{
    checkOption(opt, OptionParser::parse(opt, value));
}

void
Configurable::checkOption(const string &opt, const string &value)
{
    checkOption(Opt(util::parseEnum<OptEnum>(opt)), value);
}

void
Configurable::setOption(Opt opt, const string &value) 
{
    setOption(opt, OptionParser::parse(opt, value));
}

void
Configurable::setOption(const string &opt, const string &value)
{
    setOption(Opt(util::parseEnum<OptEnum>(opt)), value);
}

void
Configurable::resetConfig(const Defaults &defaults, isize objid)
{
    for (auto &option : getOptions()) {
        
        try {
            setOption(option, defaults.get(option, objid));
        } catch (...) {
            setOption(option, defaults.getFallback(option, objid));
        }
    }
}

void
Configurable::dumpConfig(std::ostream &os) const
{
    using namespace util;

    for (auto &opt: getOptions()) {

        auto name = OptEnum::key(opt);
        auto help = OptEnum::help(opt);
        auto arg  = OptionParser::asString(opt, getOption(opt));
        auto arghelp = OptionParser::help(opt, getOption(opt));

        os << tab(name);
        os << std::setw(16) << std::left << std::setfill(' ') << arg;
        os << help;
        if (arghelp != "") os << " (" << arghelp << ")";
        os << std::endl;
    }
}

}
