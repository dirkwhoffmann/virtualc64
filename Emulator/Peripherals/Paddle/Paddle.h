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

#include "PaddleTypes.h"
#include "JoystickTypes.h"
#include "C64Types.h"
#include "SubComponent.h"

namespace vc64 {

class Paddle final : public SubComponent, public Inspectable<PaddleInfo> {

    Descriptions descriptions = {
        {
            .name           = "Paddle 1",
            .shellName      = "paddle1",
            .description    = "Paddle in Port 1"
        },
        {
            .name           = "Paddle 2",
            .shellName      = "paddle2",
            .description    = "Paddle in Port 2"
        }
    };

    ConfigOptions options = {

    };

    // Reference to the control port this device belongs to
    ControlPort &port;

    // Current configuration
    PaddleConfig config = { };

    // Button states
    bool leftButton = false;
    bool rightButton = false;

    // Paddle position (-1 ... 1)
    double pos = 0;


    //
    // Methods
    //

public:

    Paddle(C64 &ref, ControlPort& pref);

    Paddle& operator= (const Paddle& other) {

        /*
        CLONE(leftButton)
        CLONE(rightButton)
        CLONE(pos)
        */
        CLONE(config)

        return *this;
    }


    //
    // Methods from Serializable
    //

public:

    template <class T> void serialize(T& worker) {

        if (isResetter(worker)) return;
    }
    void operator << (SerChecker &worker) override { serialize(worker); }
    void operator << (SerCounter &worker) override { serialize(worker); }
    void operator << (SerResetter &worker) override;
    void operator << (SerReader &worker) override;
    void operator << (SerWriter &worker) override { serialize(worker); }


    //
    // Methods from CoreComponent
    //

public:

    const Descriptions &getDescriptions() const override { return descriptions; }

private:

    void _dump(Category category, std::ostream& os) const override;
    void _reset(bool hard) override;


    //
    // Methods from Inspectable
    //

private:

    void cacheInfo(PaddleInfo &result) const override;


    //
    // Methods from Configurable
    //

public:

    const PaddleConfig &getConfig() const { return config; }
    const ConfigOptions &getOptions() const override { return options; }
    i64 getOption(Option opt) const override;
    void checkOption(Option opt, i64 value) override;
    void setOption(Option opt, i64 value) override;


    //
    // Using the device
    //

public:

    // Sets the button state
    void setLeftMouseButton(bool value) { leftButton = value; }
    void setRightMouseButton(bool value) { rightButton = value; }

    // Sets the paddle position
    void setX(double x);
    void setX(isize x);
    void setDx(double dx);
    void setDx(isize dx);

    // Reads the port bits that show up in the CIA's data port registers
    u8 readControlPort() const;

    // Reads the pot bits that show up in the SID registers
    u8 readPotX() const;
    u8 readPotY() const;

};

}
