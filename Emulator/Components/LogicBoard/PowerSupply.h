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

class PowerSupply : public SubComponent {
    
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

private:
    
    void _reset(bool hard) override { };

    void newserialize(util::SerChecker &worker) override { }
    void newserialize(util::SerCounter &worker) override { }
    void newserialize(util::SerResetter &worker) override { }
    void newserialize(util::SerReader &worker) override { }
    void newserialize(util::SerWriter &worker) override { }


    //
    // Configuring
    //
    
public:
    
    static PowerSupplyConfig getDefaultConfig();
    const PowerSupplyConfig &getConfig() const { return config; }
    void resetConfig() override;

    i64 getConfigItem(Option option) const;
    void setConfigItem(Option option, i64 value);

    
    //
    // Using the device
    //

public:
    
    // Returns the number of CPU cycles between two TOD increments
    Cycle todTickDelay(u8 cra);
};

}
