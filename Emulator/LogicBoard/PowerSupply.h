// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
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
    // Methods from C64Object
    //
    
    const char *getDescription() const override { return "PowerSupply"; }


    //
    // Methods from C64Component
    //

private:
    
    void _reset(bool hard) override;

private:
    
    template <class T>
    void applyToPersistentItems(T& worker)
    {
    }
    
    template <class T>
    void applyToResetItems(T& worker, bool hard = true)
    {
    }

    isize _size() override { COMPUTE_SNAPSHOT_SIZE }
    u64 _checksum() override { COMPUTE_SNAPSHOT_CHECKSUM }
    isize _load(const u8 *buffer) override { LOAD_SNAPSHOT_ITEMS }
    isize _save(u8 *buffer) override { SAVE_SNAPSHOT_ITEMS }
    
    
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
