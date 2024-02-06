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
#include "Host.h"
#include "Muxer.h"
#include "IOUtils.h"

namespace vc64 {

Host::Host(C64& ref) : SubComponent(ref)
{

}

i64
Host::getConfigItem(Option option) const
{
    switch (option) {

        case OPT_HOST_REFRESH_RATE: return i64(refreshRate);
        case OPT_HOST_SAMPLE_RATE:  return i64(sampleRate);

        default:
            fatalError;
    }
}

void
Host::setConfigItem(Option option, i64 value)
{
    switch (option) {

        case OPT_HOST_REFRESH_RATE:

            refreshRate = double(value);
            return;

        case OPT_HOST_SAMPLE_RATE:

            sampleRate = double(value);
            muxer.setSampleRate(sampleRate);
            return;

        default:
            fatalError;
    }
}

void
Host::_dump(Category category, std::ostream& os) const
{
    using namespace util;

    if (category == Category::State) {

        os << tab("Audio sample rate");
        os << flt(sampleRate) << " Hz" << std::endl;
        os << tab("Monitor refresh rate");
        os << flt(refreshRate) << " Hz" << std::endl;
        os << tab("Frame buffer size");
        os << dec(frameBufferWidth) << " x ";
        os << dec(frameBufferHeight) << " Texels" << std::endl;
    }
}

void
Host::setSampleRate(double hz)
{
    sampleRate = hz;
    muxer.setSampleRate(hz);
}

void
Host::setHostRefreshRate(double fps)
{
    switch (i16(fps)) {

        case 50: case 60: case 100: case 120: case 200: case 240:

            refreshRate = fps;
            // muxer.adjustSpeed();
            break;

        default:

            // We keep the old value because the new value is likely the result
            // of a wrong measurement.
            break;
    }
}

std::pair<isize, isize>
Host::getFrameBufferSize() const
{
    return std::pair<isize, isize>(frameBufferWidth, frameBufferHeight);
}

void
Host::setFrameBufferSize(std::pair<isize, isize> size)
{
    frameBufferWidth = size.first;
    frameBufferHeight = size.second;
}

}
