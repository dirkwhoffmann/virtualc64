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

#include "PowerPortTypes.h"
#include "SubComponent.h"
#include "Chrono.h"

namespace vc64 {

class PowerPort final : public SubComponent, public Dumpable {

    Descriptions descriptions = {{

        .name           = "PowerPort",
        .shellName      = "power",
        .description    = "Power Port"
    }};

    ConfigOptions options = {

        OPT_POWER_GRID
    };

    PowerPortConfig config = { };


    //
    // Methods
    //
    
public:
    
    PowerPort(C64& ref);
    const Descriptions &getDescriptions() const override { return descriptions; }
    PowerPort& operator= (const PowerPort& other) { return *this; }
    template <class T> void serialize(T& worker) { } SERIALIZERS(serialize);


    //
    // Configuring
    //

public:

    const PowerPortConfig &getConfig() const { return config; }
    const ConfigOptions &getOptions() const override { return options; }
    i64 getOption(Option opt) const override;
    void checkOption(Option opt, i64 value) override;
    void setOption(Option opt, i64 value) override;


    //
    // Inspecting
    //

    void _dump(Category category, std::ostream& os) const override;


    //
    // Using the device
    //

public:
    
    // Returns the number of CPU cycles between two TOD increments
    Cycle todTickDelay(u8 cra);
};

}
