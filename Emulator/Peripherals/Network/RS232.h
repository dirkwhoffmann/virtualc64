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

#include "RS232Types.h"
#include "SubComponent.h"

namespace vc64 {

class RS232 final : public SubComponent {

    Descriptions descriptions = {{

        .name           = "RS232",
        .description    = "RS232 User-Port Adapter"
    }};

    ConfigOptions options = {

        OPT_RS232_BAUD
    };

    // Current configuration
    RS232Config config = { };


    //
    // Methods
    //

public:

    using SubComponent::SubComponent;

    RS232& operator= (const RS232& other) {

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

        << config.baud;

    } SERIALIZERS(serialize);


    //
    // Methods from CoreComponent
    //

public:

    const Descriptions &getDescriptions() const override { return descriptions; }

private:

    void _dump(Category category, std::ostream& os) const override;


    //
    // Methods from Inspectable
    //

private:

    // void cacheInfo(RS232Info &result) const override;


    //
    // Methods from Configurable
    //

public:

    const RS232Config &getConfig() const { return config; }
    const ConfigOptions &getOptions() const override { return options; }
    i64 getOption(Option opt) const override;
    void checkOption(Option opt, i64 value) override;
    void setOption(Option opt, i64 value) override;

};

}
