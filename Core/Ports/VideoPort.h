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

#include "VideoPortTypes.h"
#include "SubComponent.h"
#include "Texture.h"

namespace vc64 {

class VideoPort final : public SubComponent, public Inspectable<VideoPortInfo, VideoPortStats> {

    Descriptions descriptions = {{

        .type           = Class::VideoPort,
        .name           = "Video",
        .description    = "Video Port",
        .shell          = "video"
    }};

    Options options = {

        Opt::VID_WHITE_NOISE
    };

    // Current configuration
    VideoPortConfig config = { };

    // Predefined frame buffers
    mutable Texture whiteNoise;
    Texture blank;

    //  White noise data
    util::Buffer <Texel> noise;

    //
    // Methods
    //

public:

    VideoPort(C64 &ref);
    ~VideoPort();
    
    const Descriptions &getDescriptions() const override { return descriptions; }

    VideoPort& operator= (const VideoPort& other) {

        CLONE(config)

        return *this;
    }


    //
    // Methods from Serializable
    //

public:
    
    template <class T>
    void serialize(T& worker)
    {
        if (isResetter(worker)) return;

        worker

        << config.whiteNoise;

    } SERIALIZERS(serialize);


    //
    // Methods from CoreComponent
    //

private:

    void _dump(Category category, std::ostream &os) const override;


    //
    // Methods from Configurable
    //
    
public:

    const VideoPortConfig &getConfig() const { return config; }
    const Options &getOptions() const override { return options; }
    i64 getOption(Opt opt) const override;
    void checkOption(Opt opt, i64 value) override;
    void setOption(Opt opt, i64 value) override;


    //
    // Getting textures
    //

public:

    // Returns a pointer to the emulator texture
    const class Texture &getTexture(isize offset = 0) const;

    // Returns a pointer to the bus debugger texture
    const class Texture &getDmaTexture(isize offset = 0) const;

    // Informs the video port about a buffer swap
    void buffersWillSwap();

    // Determines the active texture area by auto-detecting border pixels
    void findInnerArea(isize &x1, isize &x2, isize &y1, isize &y2) const;
    void findInnerAreaNormalized(double &x1, double &x2, double &y1, double &y2) const;

private:

    // Returns a pointer to a white-noise texture
    u32 *getNoiseTexture() const;

    // Returns a pointer to a solid blank texture
    u32 *getBlankTexture() const;
};

}
