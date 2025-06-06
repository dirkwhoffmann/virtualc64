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
#include "VideoPort.h"
#include "VICII.h"

namespace vc64 {

VideoPort::VideoPort(C64 &ref) : SubComponent(ref)
{
    // Create random noise data
    noise.alloc(2 * Texture::texels);
    for (isize i = 0; i < noise.size; i++) {
        noise[i] = rand() % 2 ? Texture::black : Texture::white;
    }

    // Create the white-noise texture, redirecting the data source
    whiteNoise.pixels.dealloc();
    whiteNoise.pixels.ptr = noise.ptr;
    whiteNoise.pixels.size = Texture::texels;

    // Create the blank texture
    for (isize i = 0; i < blank.pixels.size; i++) {
        blank.pixels.ptr[i] = Texture::black;
    }
};

VideoPort::~VideoPort()
{
    // Don't free the white-noise framebuffer as it points to managed data
    whiteNoise.pixels.ptr = nullptr;
    whiteNoise.pixels.size = 0;
}

void
VideoPort::_dump(Category category, std::ostream &os) const
{
    using namespace util;

    if (category == Category::Config) {

        dumpConfig(os);
    }

    if (category == Category::State) {

    }
}

i64
VideoPort::getOption(Opt option) const
{
    switch (option) {

        case Opt::VID_WHITE_NOISE:   return config.whiteNoise;

        default:
            fatalError;
    }
}

void
VideoPort::checkOption(Opt opt, i64 value)
{
    switch (opt) {

        case Opt::VID_WHITE_NOISE:

            return;

        default:
            throw AppError(Fault::OPT_UNSUPPORTED);
    }
}

void
VideoPort::setOption(Opt opt, i64 value)
{
    checkOption(opt, value);

    switch (opt) {

        case Opt::VID_WHITE_NOISE:

            config.whiteNoise = (bool)value;
            return;

        default:
            fatalError;
    }
}

const class Texture &
VideoPort::getTexture(isize offset) const
{
    if (isPoweredOn()) {

        auto &result = vic.getStableBuffer(offset);
        info.latestGrabbedFrame = result.nr;
        return result;
    }
    if (config.whiteNoise) {

        whiteNoise.pixels.ptr = noise.ptr + (rand() % Texture::texels);
        whiteNoise.nr++;

        return whiteNoise;
    }

    return blank;
}

const class Texture &
VideoPort::getDmaTexture(isize offset) const
{
    if (isPoweredOn()) {

        return vic.getStableDmaBuffer(offset);

    } else {

        return blank;
    }
}

void
VideoPort::buffersWillSwap()
{
    // Check if the texture has been grabbed
    auto grabbed = info.latestGrabbedFrame;
    auto current = vic.getStableBuffer().nr;

    if (grabbed < current) {

        stats.droppedFrames++;
        debug(TIM_DEBUG, "Frame %lld dropped (total: %ld latest: %lld)\n",
            current, stats.droppedFrames, grabbed);
    }
}

u32 *
VideoPort::getNoiseTexture() const
{
    static u32 *noise = nullptr;
    constexpr isize noiseSize = 16 * 512 * 512;

    if (!noise) {

        noise = new u32[noiseSize];

        for (isize i = 0; i < noiseSize; i++) {
            noise[i] = rand() % 2 ? 0xFF000000 : 0xFFFFFFFF;
        }
    }

    int offset = rand() % (512 * 512);
    return noise + offset;
}

u32 *
VideoPort::getBlankTexture() const
{
    static u32 *blank = nullptr;

    if (!blank) {

        blank = new u32[Texture::height * Texture::width];

        for (isize i = 0; i < Texture::height * Texture::width; i++) {
            blank[i] = 0xFF000000;
        }
    }

    return blank;
}

}
