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

#include "JoystickTypes.h"
#include "C64Types.h"
#include "SubComponent.h"

namespace vc64 {

class Joystick final : public SubComponent, public Inspectable<JoystickInfo> {

    Descriptions descriptions = {
        {
            .type           = JoystickClass,
            .name           = "Joystick1",
            .description    = "Joystick in Port 1",
            .shell          = "joystick1"
        },
        {
            .type           = JoystickClass,
            .name           = "Joystick2",
            .description    = "Joystick in Port 2",
            .shell          = "joystick2"
        }
    };

    Options options = {

        OPT_AUTOFIRE,
        OPT_AUTOFIRE_BURSTS,
        OPT_AUTOFIRE_BULLETS,
        OPT_AUTOFIRE_DELAY
    };
    
    // Reference to the control port this device belongs to
    ControlPort &port;

    // Current configuration
    JoystickConfig config = { };
    
    // Button state
    bool button = false;
    
    // Horizontal joystick position (-1 = left, 1 = right, 0 = released)
    int axisX = 0;
    
    // Vertical joystick position (-1 = up, 1 = down, 0 = released)
    int axisY = 0;

    // Bullet counter used in autofire mode
    isize bulletCounter = 0;

    // Next frame to auto-press or auto-release the fire button
    i64 nextAutofireFrame = 0;
    i64 nextAutofireReleaseFrame = 0;


    //
    // Methods
    //
    
public:
    
    Joystick(C64 &ref, ControlPort& pref);

    Joystick& operator= (const Joystick& other) {

        CLONE(config)

        CLONE(button)
        CLONE(axisX)
        CLONE(axisY)
        CLONE(bulletCounter)
        CLONE(nextAutofireFrame)
        CLONE(nextAutofireReleaseFrame)

        return *this;
    }


    //
    // Methods from Serializable
    //

public:

    template <class T> void serialize(T& worker) {

        if (isResetter(worker)) {

            worker

            << button
            << axisX
            << axisY
            << bulletCounter
            << nextAutofireFrame
            << nextAutofireReleaseFrame;

        } else {
            
            /*
            worker
            
            << config.autofire
            << config.autofireBursts
            << config.autofireBullets
            << config.autofireDelay;
            */
        }
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


    //
    // Methods from Inspectable
    //

private:

    void cacheInfo(JoystickInfo &result) const override;


    //
    // Methods from Configurable
    //

public:

    const JoystickConfig &getConfig() const { return config; }
    const Options &getOptions() const override { return options; }
    i64 getOption(Option opt) const override;
    void checkOption(Option opt, i64 value) override;
    void setOption(Option opt, i64 value) override;

    
    //
    // Using the device
    //
    
public:

    // Reads the port bits that show up in the CIA's data port registers
    u8 getControlPort() const;

    // Triggers a gamepad event
    void trigger(GamePadAction event);

    // To be called after each frame
    void eofHandler();


    //
    // Auto fire
    //

    // Checks whether autofiring is active
    bool isAutofiring();

    // Returns the maximum number of bullets to fire
    isize magazineSize();

    // Starts or stops autofire mode
    void startAutofire();
    void stopAutofire();

    // Reloads the autofire magazine
    void reload();
};

}
