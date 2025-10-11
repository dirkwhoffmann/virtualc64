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

#include "SubComponent.h"
#include "Chrono.h"

namespace vc64 {

class SampleRateDetector final : public SubComponent {

    Descriptions descriptions = {{

        .type           = Class::SampleRateDetector,
        .name           = "SampleRateDetector",
        .description    = "Sample rate detector",
        .shell          = ""
    }};

    Options options = {

    };

    // Number of requested audio samples
    isize count = 0;

    // Measured timespan between two requests
    util::Clock delta;

    // Ring buffer storing the latest measurements
    util::RingBuffer<double, 256> buffer;

    // Number of filtered out outliers at both ends
    const isize trash = 6;


    //
    // Methods
    //

public:

    using SubComponent::SubComponent;

    SampleRateDetector& operator= (const SampleRateDetector& other) {

        return *this;
    }


    //
    // Methods from CoreObject
    //

private:

    void _dump(Category category, std::ostream &os) const override;


    //
    // Methods from CoreComponent
    //

private:

    template <class T>
    void serialize(T& worker)
    {

    } SERIALIZERS(serialize);

    void _didReset(bool hard) override;

public:

    const Descriptions &getDescriptions() const override { return descriptions; }


    //
    // Methods from Configurable
    //

public:

    const Options &getOptions() const override { return options; }


    //
    // Running the device
    //

    // Informs the detector that sound samples have been requested
    void feed(isize samples);

    // Returns the current sample rate
    double sampleRate();

};

}
