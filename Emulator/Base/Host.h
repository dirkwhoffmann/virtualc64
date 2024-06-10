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

#include "CoreComponent.h"
#include "IOUtils.h"

typedef struct
{
    // Refresh rate of the host display
    isize refreshRate;

    // Audio sample rate of the host computer
    isize sampleRate;

    // Framebuffer dimensions
    isize frameBufferWidth;
    isize frameBufferHeight;
}
HostConfig;

namespace vc64 {

/* This class stores some information about the host system. The values have
 * are set by the GUI on start and updated on-the-fly when a value changes.
 */
class Host final : public CoreComponent {

    Descriptions descriptions = {{

        .name           = "Host",
        .description    = "Host Computer"
    }};

    ConfigOptions options = {

        OPT_HOST_REFRESH_RATE,
        OPT_HOST_SAMPLE_RATE,
        OPT_HOST_FRAMEBUF_WIDTH,
        OPT_HOST_FRAMEBUF_HEIGHT
    };
    
    // Current configuration
    HostConfig config = { };


    //
    // Methods
    //

public:

    using CoreComponent::CoreComponent;

    Host& operator= (const Host& other) {

        CLONE(config)
        return *this;
    }


    //
    // Methods from Serializable
    //

public:

    template <class T> void serialize(T& worker) { } SERIALIZERS(serialize);


    //
    // Methods from CoreComponent
    //

public:

    const Descriptions &getDescriptions() const override { return descriptions; }

private:

    void _dump(Category category, std::ostream& os) const override;


    //
    // Configuring
    //

public:

    const HostConfig &getConfig() const { return config; }
    const ConfigOptions &getOptions() const override { return options; }
    i64 getOption(Option opt) const override;
    void checkOption(Option opt, i64 value) override;
    void setOption(Option opt, i64 value) override;


    //
    // Working with temporary files and folders
    //

public:
    
    // Returns a path to a temporary folder
    fs::path tmp() const throws;

    // Assembles a path to a temporary file
    fs::path tmp(const string &name, bool unique = false) const throws;
};

}

