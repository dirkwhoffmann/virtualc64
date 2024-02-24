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

namespace vc64 {

/* This class stores some information about the host system. The values have
 * are set by the GUI on start and updated on-the-fly when a value changes.
 */
class Host final : public CoreComponent, public Dumpable {

    ConfigOptions options = {

        { OPT_HOST_REFRESH_RATE,    "Video refresh rate" },
        { OPT_HOST_SAMPLE_RATE,     "Audio sample rate" },
        { OPT_HOST_FRAMEBUF_WIDTH,  "Window height" },
        { OPT_HOST_FRAMEBUF_HEIGHT, "Window width" }
    };
    
    // Refresh rate of the host display
    double refreshRate = 60.0;

    // Audio sample rate of the host computer
    double sampleRate = 44100.0;

    // Framebuffer dimensions
    isize frameBufferWidth = 0;
    isize frameBufferHeight = 0;


    //
    // Initializing
    //

public:

    using CoreComponent::CoreComponent;


    //
    // Methods from CoreObject
    //

private:

    const char *getDescription() const override { return "Host"; }
    void _dump(Category category, std::ostream& os) const override;

    Host& operator= (const Host& other) {

        CLONE(refreshRate)
        CLONE(sampleRate)
        CLONE(frameBufferWidth)
        CLONE(frameBufferHeight)

        return *this;
    }

    template <class T> void serialize(T& worker) { } SERIALIZERS(serialize);


    //
    // Methods from Configurable
    //

public:

    const ConfigOptions &getOptions() const override { return options; }

    // Gets or sets a config option
    i64 getOption(Option opt) const override;
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

