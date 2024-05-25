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

namespace vc64 {

class VideoPort final : public SubComponent, public Dumpable {

    Descriptions descriptions = {{

        .name           = "VideoPort",
        .shellName      = "video",
        .description    = "Video Port"
    }};

    ConfigOptions options = {

        OPT_VID_WHITE_NOISE
    };

    // Current configuration
    VideoPortConfig config = { };


    //
    // Methods
    //

public:

    VideoPort(C64 &ref) : SubComponent(ref) { };
    const Descriptions &getDescriptions() const override { return descriptions; }
    void _dump(Category category, std::ostream& os) const override;

    VideoPort& operator= (const VideoPort& other) {

        CLONE(config)

        return *this;
    }

    template <class T>
    void serialize(T& worker)
    {
        if (isResetter(worker)) return;

        worker

        << config.whiteNoise;

    } SERIALIZERS(serialize);


    //
    // Configuring
    //

public:

    const VideoPortConfig &getConfig() const { return config; }
    const ConfigOptions &getOptions() const override { return options; }
    i64 getOption(Option opt) const override;
    void checkOption(Option opt, i64 value) override;
    void setOption(Option opt, i64 value) override;


    //
    // Getting textures
    //

public:

    // Returns a pointer to the stable emulator texture
    u32 *getTexture() const;

    // Returns a pointer to the stable DMA debugger texture
    u32 *getDmaTexture() const;

    // DEPRECATED
    u32 *getNoise() const;
};

}
