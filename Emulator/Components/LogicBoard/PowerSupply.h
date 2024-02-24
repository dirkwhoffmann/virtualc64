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

#include "PowerSupplyTypes.h"
#include "SubComponent.h"
#include "Chrono.h"

namespace vc64 {

class PowerSupply final : public SubComponent {
    
    ConfigOptions options = {

        { OPT_POWER_GRID,   "Power grid" }
    };

    // Current configuration
    PowerSupplyConfig config = { };

    
    //
    // Initializing
    //
    
public:
    
    PowerSupply(C64& ref);
    
    
    //
    // Methods from CoreObject
    //
    
    const char *getDescription() const override { return "PowerSupply"; }


    //
    // Methods from CoreComponent
    //

public:

    PowerSupply& operator= (const PowerSupply& other) { return *this; }
    template <class T> void serialize(T& worker) { } SERIALIZERS(serialize);


    //
    // Methods from Configurable
    //

    const ConfigOptions &getOptions() const override { return options; }

    // Gets or sets a config option
    i64 getOption(Option opt) const override;
    void setOption(Option opt, i64 value) override;


    //
    // Configuring
    //
    
public:
    
    const PowerSupplyConfig &getConfig() const { return config; }

    
    //
    // Using the device
    //

public:
    
    // Returns the number of CPU cycles between two TOD increments
    Cycle todTickDelay(u8 cra);
};

}
