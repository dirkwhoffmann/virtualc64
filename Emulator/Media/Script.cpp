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
#include "Script.h"
#include "C64.h"
#include "IOUtils.h"

#include <sstream>

namespace vc64 {

class C64;

bool
Script::isCompatible(const string &path)
{
    auto s = util::extractSuffix(path);
    return s == "ini" || s == "INI";
}

bool
Script::isCompatible(std::istream &stream)
{
    return true;
}

void
Script::execute(C64 &c64)
{
    string s((char *)data, size);
    try { c64.retroShell.execScript(s); } catch (util::Exception &) { }
}

}
