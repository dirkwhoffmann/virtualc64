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

#include "MouseTypes.h"
#include "JoystickTypes.h"
#include "SubComponent.h"
#include "Chrono.h"
#include "Mouse1350.h"
#include "Mouse1351.h"
#include "NeosMouse.h"

namespace vc64 {

class ShakeDetector {
    
    // Horizontal position
    double x = 0.0;
    
    // Moved distance
    double dxsum = 0.0;

    // Direction (1 or -1)
    double dxsign = 1.0;
    
    // Number of turns
    isize dxturns = 0;
    
    // Time stamps
    u64 lastTurn = 0;
    util::Time lastShake;
    
public:
    
    // Feed in new coordinates and checks for a shake
    bool isShakingAbs(double x);
    bool isShakingRel(double dx);
};

class Mouse final : public SubComponent {
    
    Descriptions descriptions = {
        {
            .type           = MouseClass,
            .name           = "Mouse1",
            .description    = "Mouse in Port 1",
            .shell          = "mouse1"
        },
        {
            .type           = MouseClass,
            .name           = "Mouse2",
            .description    = "Mouse in Port 2",
            .shell          = "mouse2"
        }
    };

    Options options = {

        OPT_MOUSE_MODEL,
        OPT_MOUSE_SHAKE_DETECT,
        OPT_MOUSE_VELOCITY
    };

    // Reference to the control port this device belongs to
    ControlPort &port;

    // Current configuration
    MouseConfig config = { };

    
    //
    // Subcomponents
    //

    // Shake detector
    class ShakeDetector shakeDetector;

    // A Commdore 1350 (digital) mouse
    Mouse1350 mouse1350 = Mouse1350(c64);
    
    // A Commdore 1351 (analog) mouse
    Mouse1351 mouse1351 = Mouse1351(c64);
    
    // A Neos (analog) mouse
    NeosMouse mouseNeos = NeosMouse(c64);

    /* Target mouse position. In order to achieve a smooth mouse movement, a
     * new mouse coordinate is not written directly into mouseX and mouseY.
     * Instead, these variables are set. In execute(), mouseX and mouseY are
     * shifted smoothly towards the target positions.
     */
    double targetX = 0.0;
    double targetY = 0.0;

    // Scaling factors applied to the raw mouse coordinates in setXY()
    double scaleX = 1.0;
    double scaleY = 1.0;

    
    //
    // Methods
    //
    
public:
    
    Mouse(C64 &ref, ControlPort& pref);

    Mouse& operator= (const Mouse& other) {

        CLONE(targetX)
        CLONE(targetY)
        CLONE(scaleX)
        CLONE(scaleY)

        CLONE(mouse1350)
        CLONE(mouse1351)
        CLONE(mouseNeos)

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

        << config.model;

    } SERIALIZERS(serialize)


    //
    // Methods from CoreComponent
    //

public:

    const Descriptions &getDescriptions() const override { return descriptions; }

private:

    void _dump(Category category, std::ostream& os) const override;
    void _didReset(bool hard) override;


    //
    // Configuring
    //

public:

    const MouseConfig &getConfig() const { return config; }
    const Options &getOptions() const override { return options; }
    i64 getOption(Option opt) const override;
    void checkOption(Option opt, i64 value) override;
    void setOption(Option opt, i64 value) override;

private:
    
    void updateScalingFactors();
    

    //
    // Accessing
    //
    
public:

    // Runs the shake detector
    bool detectShakeXY(double x, double y);
    bool detectShakeDxDy(double dx, double dy);

    // Emulates a mouse movement
    void setXY(double x, double y);
    void setDxDy(double dx, double dy);

    // Presses or releases a mouse button
    void setLeftButton(bool value);
    void setRightButton(bool value);
    
    // Triggers a gamepad event
    void trigger(GamePadAction event);
    
    // Triggers a state change (Neos mouse only)
    void risingStrobe();
    void fallingStrobe();
    
    // Updates the control port bits (must be called before reading)
    void updateControlPort();
    
    // Returns the control port bits as set by the mouse
    u8 getControlPort() const;

    // Updates the pot bits (must be called before reading)
    void updatePotX();
    void updatePotY();

    // Reads the pot bits that show up in the SID registers
    u8 readPotX() const;
    u8 readPotY() const;

    // Performs periodic actions for this device
    void execute();
};

}
