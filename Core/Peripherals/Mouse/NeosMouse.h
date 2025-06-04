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

#include "SubComponent.h"

namespace vc64 {

class NeosMouse final : public SubComponent {
    
    Descriptions descriptions = {{

        .name           = "NeosMouse",
        .description    = "Neos Mouse",
        .shell          = ""
    }};

    Options options = {

    };
    
    // Mouse position
    i64 mouseX;
    i64 mouseY;
    
    // Mouse button states
    bool leftButton;
    bool rightButton;
    
    // Dividers applied to raw coordinates in setXY()
    int dividerX = 512;
    int dividerY = 256;
    
    // Mouse movement in pixels per execution step
    i64 shiftX = 127;
    i64 shiftY = 127;
    
    // Mouse state. When the mouse switches to state 0, the current mouse
    // position is latched and the deltaX and deltaY are computed. After that,
    // the mouse cycles through the other states and writes the delta values
    // onto the control port, nibble by nibble.
    u8 state;

    // CPU cycle of the most recent trigger event
    i64 triggerCycle;
    
    // Latched mouse position
    i64 latchedX;
    i64 latchedY;
    
    // Value transmitted to the C64
    i8 deltaX;
    i8 deltaY;
    
    
    //
    // Methods
    //
    
public:
    
    NeosMouse(C64 &ref) : SubComponent(ref) { }

    NeosMouse& operator= (const NeosMouse& other) {

        CLONE(mouseX)
        CLONE(mouseY)
        CLONE(leftButton)
        CLONE(rightButton)
        CLONE(dividerX)
        CLONE(dividerY)
        CLONE(shiftX)
        CLONE(shiftY)
        CLONE(state)
        CLONE(triggerCycle)
        CLONE(latchedX)
        CLONE(latchedY)
        CLONE(deltaX)
        CLONE(deltaY)

        return *this;
    }

    
    //
    // Methods from Serializable
    //

public:

    template <class T> void serialize(T& worker) { } SERIALIZERS(serialize);
    void _didReset(bool hard) override;


    //
    // Methods from CoreComponent
    //

public:

    const Descriptions &getDescriptions() const override { return descriptions; }


    //
    // Methods from Configurable
    //

public:

    const Options &getOptions() const override { return options; }
    

    //
    // Accessing
    //
    
public:
    
    // Updates the button state
    void setLeftMouseButton(bool value) { leftButton = value; }
    void setRightMouseButton(bool value) { rightButton = value; }
    
    // Returns the pot bits as set by the mouse
    u8 readPotX() const;
    u8 readPotY() const;
    
    // Updates the control port bits (must be called before reading)
    void updateControlPort(i64 targetX, i64 targetY);
    
    // Returns the control port bits triggered by the mouse
    u8 readControlPort() const;

    // Triggers a state change
    void risingStrobe(i64 targetX, i64 targetY);
    void fallingStrobe(i64 targetX, i64 targetY);
    
private:
    
    // Latches the current mouse position and computed the transmission deltas
    void latchPosition(i64 targetX, i64 targetY);
};

}
