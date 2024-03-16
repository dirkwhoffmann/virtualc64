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
#include "Configurable.h"
#include "Option.h"
#include "Parser.h"
#include "VICIITypes.hpp"
#include "DatasetteTypes.hpp"
#include "MemoryTypes.hpp"
#include "SIDTypes.hpp"
#include "CIATypes.hpp"
#include "DriveTypes.hpp"
#include "ParCableTypes.hpp"
#include "PowerSupplyTypes.hpp"
#include "MouseTypes.hpp"
#include "IOUtils.h"
#include <algorithm>

namespace vc64 {

using namespace util;

bool
Configurable::isValidOption(Option opt) const
{
    for (auto &it: getOptions()) {
        if (it == opt) return true;
    }
    return false;
}

void
Configurable::setOption(Option opt, const string &value) 
{
    setOption(opt, OptionParser::create(opt)->parse(value));
}

void
Configurable::setOption(const string &opt, const string &value)
{
    setOption(Option(util::parseEnum<OptionEnum>(opt)), value);
}

void
Configurable::resetConfig(const Defaults &defaults)
{
    for (auto &option : getOptions()) {
        setOption(option, defaults.get(option));
    }
}

void
Configurable::resetConfig(const Defaults &defaults, isize i)
{
    for (auto &option : getOptions()) {
        setOption(option, defaults.get(option, i));
    }
}

void
Configurable::dumpConfig(std::ostream& os) const
{
    using namespace util;

    for (auto &opt: getOptions()) {

        auto name = OptionEnum::plainkey(opt);
        auto help = OptionEnum::help(opt);
        auto arg  = OptionParser::create(opt, getOption(opt))->asString();

        os << tab(name);
        os << std::setw(16) << std::left << std::setfill(' ') << arg;
        os <<help << std::endl;
    }
}

}
