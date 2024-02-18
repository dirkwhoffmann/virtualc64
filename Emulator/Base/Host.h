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

namespace vc64 {

/* This class stores some information about the host system. The values have
 * are set by the GUI on start and updated on-the-fly when a value changes.
 */
class Host : public CoreComponent, public Dumpable {

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

    void newserialize(util::SerChecker &worker) override { }
    void newserialize(util::SerCounter &worker) override { }
    void newserialize(util::SerResetter &worker) override { }
    void newserialize(util::SerReader &worker) override { }
    void newserialize(util::SerWriter &worker) override { }
    

    //
    // Methods from CoreComponent
    //

private:

    void _reset(bool hard) override { };


    //
    // Configuring
    //

public:

    i64 getConfigItem(Option option) const;
    void setConfigItem(Option option, i64 value);
};

}

