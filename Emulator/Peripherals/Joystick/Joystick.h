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
#include "SubComponent.h"

namespace vc64 {

class Joystick : public SubComponent, public Dumpable {
    
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

    // Bullet counter used in multi-fire mode
    i64 bulletCounter = 0;
    
    // Next frame to auto-press or auto-release the fire button
    i64 nextAutofireFrame = 0;
    
    
    //
    // Initializing
    //
    
public:
    
    Joystick(C64 &ref, ControlPort& pref);
    
    
    //
    // Methods from CoreObject
    //

private:
    
    const char *getDescription() const override;
    void _dump(Category category, std::ostream& os) const override;

    
    //
    // Methods from CoreComponent
    //

private:
    
    void _reset(bool hard) override;
    
    template <class T>
    void serialize(T& worker)
    {
    }
    
    void operator << (util::SerChecker &worker) override { serialize(worker); }
    void operator << (util::SerCounter &worker) override { serialize(worker); }
    void operator << (util::SerResetter &worker) override { serialize(worker); }
    void operator << (util::SerReader &worker) override;
    void operator << (util::SerWriter &worker) override { serialize(worker); }


    //
    // Configuring
    //
    
public:

    const JoystickConfig &getConfig() const { return config; }
    void resetConfig() override;

    i64 getConfigItem(Option option) const;
    void setConfigItem(Option option, i64 value);
    
    
    //
    // Using the device
    //
    
public:

    // Reads the port bits that show up in the CIA's data port registers
    u8 getControlPort() const;

    // Triggers a gamepad event
    void trigger(GamePadAction event);

    /* Execution function for this control port. This method needs to be
     * invoked at the end of each frame to make the auto-fire mechanism work.
     */
    void execute();
    
private:
    
    // Reloads the autofire magazine
    void reload();
    
    // Updates variable nextAutofireFrame
    void scheduleNextShot();
};

}
