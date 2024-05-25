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

#include "config.h"
#include "VideoPort.h"
#include "VICII.h"

namespace vc64 {

void
VideoPort::_dump(Category category, std::ostream& os) const
{
    using namespace util;

    if (category == Category::Config) {

        dumpConfig(os);
    }

    if (category == Category::State) {

    }
}

u32 *
VideoPort::getTexture() const
{
    return vic.getTexture();
}

u32 *
VideoPort::getDmaTexture() const
{
    return vic.getDmaTexture();
}

u32 *
VideoPort::getNoise() const
{
    return vic.getNoise();
}

}
