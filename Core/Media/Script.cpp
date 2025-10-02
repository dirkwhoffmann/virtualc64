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
#include "VirtualC64.h"
#include "IOUtils.h"

#include <sstream>

namespace vc64 {

bool
Script::isCompatible(const fs::path &path)
{
    auto s = util::uppercased(path.extension().string());
    return s == ".INI";
}

bool
Script::isCompatible(const u8 *buf, isize len)
{
    return true;
}

bool
Script::isCompatible(const Buffer<u8> &buf)
{
    return isCompatible(buf.ptr, buf.size);
}

}
