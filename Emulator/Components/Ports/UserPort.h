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

#include "UserPortTypes.h"
#include "SubComponent.h"
#include "RS232.h"

namespace vc64 {

class UserPort final : public SubComponent {

    Descriptions descriptions = {{

        .type           = UserPortClass,
        .name           = "User",
        .description    = "User Port",
        .shell          = "user"
    }};

    Options options = {

        Opt::USR_DEVICE
    };

    // Current configuration
    UserPortConfig config = { };

    //
    // Subcomponents
    //

public:
    
    // RS232 adapter
    RS232 rs232 = RS232(c64);


    //
    // Methods
    //

public:

    UserPort(C64 &ref);
    const Descriptions &getDescriptions() const override { return descriptions; }

    UserPort& operator= (const UserPort& other) {

        CLONE(rs232)
        CLONE(config)

        return *this;
    }


    //
    // Methods from Serializable
    //

public:

    template <class T>
    void serialize(T& worker)
    {
        if (isResetter(worker)) return;

        worker

        << config.device;

    } SERIALIZERS(serialize);


    //
    // Methods from CoreComponent
    //

private:

    void _dump(Category category, std::ostream& os) const override;


    //
    // Methods from Configurable
    //

public:

    const UserPortConfig &getConfig() const { return config; }
    const Options &getOptions() const override { return options; }
    i64 getOption(Opt opt) const override;
    void checkOption(Opt opt, i64 value) override;
    void setOption(Opt opt, i64 value) override;


    //
    // Reading and writing port values
    //

public:

    /* Returns the value of the PB pins.
     * This functions returns the values of the PB pins as they would appear
     * if all CIA pins were configured as inputs.
     */
    u8 getPB() const;

    // Signals a change on the PA2 pin
    void setPA2(bool value);
};

}
