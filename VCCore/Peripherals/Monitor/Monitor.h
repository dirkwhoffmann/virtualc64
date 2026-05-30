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

#include "MonitorTypes.h"
#include "C64Types.h"
#include "SubComponent.h"

namespace vc64 {

class Monitor final : public SubComponent {

    Descriptions descriptions = {
        {
            .type           = Class::Monitor,
            .name           = "Monitor",
            .description    = "Computer Monitor",
            .shell          = "monitor"
        }
    };

    Options options = {

        Opt::MON_PALETTE,
        Opt::MON_BRIGHTNESS,
        Opt::MON_CONTRAST,
        Opt::MON_SATURATION,
        Opt::MON_HCENTER,
        Opt::MON_VCENTER,
        Opt::MON_HZOOM,
        Opt::MON_VZOOM,
        Opt::MON_UPSCALER,
        Opt::MON_BLUR,
        Opt::MON_BLUR_RADIUS,
        Opt::MON_BLOOM,
        Opt::MON_BLOOM_RADIUS,
        Opt::MON_BLOOM_BRIGHTNESS,
        Opt::MON_BLOOM_WEIGHT,
        Opt::MON_DOTMASK,
        Opt::MON_DOTMASK_BRIGHTNESS,
        Opt::MON_SCANLINES,
        Opt::MON_SCANLINE_BRIGHTNESS,
        Opt::MON_SCANLINE_WEIGHT,
        Opt::MON_DISALIGNMENT,
        Opt::MON_DISALIGNMENT_H,
        Opt::MON_DISALIGNMENT_V
    };

    // Current configuration
    MonitorConfig config = { };


    //
    // Methods
    //

public:

    Monitor(C64 &ref) : SubComponent(ref) { }

    Monitor& operator= (const Monitor& other) {

        CLONE(config)

        return *this;
    }

    //
    // Methods from Serializable
    //

public:

    template <class T> void serialize(T& worker) {

        if (isResetter(worker)) return;

        worker

        << config.palette
        << config.brightness
        << config.contrast
        << config.saturation

        << config.hCenter
        << config.vCenter
        << config.hZoom
        << config.vZoom

        << config.upscaler

        << config.blur
        << config.blurRadius

        << config.bloom
        << config.bloomRadius
        << config.bloomBrightness
        << config.bloomWeight

        << config.dotmask
        << config.dotMaskBrightness

        << config.scanlines
        << config.scanlineBrightness
        << config.scanlineWeight

        << config.disalignment
        << config.disalignmentH
        << config.disalignmentV;


    } SERIALIZERS(serialize);


    //
    // Methods from CoreComponent
    //

public:

    const Descriptions &getDescriptions() const override { return descriptions; }

private:

    void _dump(Category category, std::ostream &os) const override;


    //
    // Methods from Configurable
    //

public:

    const MonitorConfig &getConfig() const { return config; }
    const Options &getOptions() const override { return options; }
    i64 getOption(Opt opt) const override;
    void checkOption(Opt opt, i64 value) override;
    void setOption(Opt opt, i64 value) override;


    //
    // Computing color values
    //

public:

    // Computes a C64 color in 32 bit big-endian RGBA format
    u32 getColor(isize nr, Palette palette);
    u32 getColor(isize nr) { return getColor(nr, config.palette); }
};

}
