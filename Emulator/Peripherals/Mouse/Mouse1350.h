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

class Mouse1350 final : public SubComponent {
    
    Descriptions descriptions = {{

        .name           = "Mouse1350",
        .shellName      = "",
        .description    = "Mouse 1350"
    }};

private:
    
    // Mouse position
    i64 mouseX;
    i64 mouseY;
    
    // Mouse button states
    bool leftButton;
    bool rightButton;
    
    // Dividers applied to raw coordinates in setXY()
    int dividerX = 64;
    int dividerY = 64;
    
    // Latched mouse positions
    i64 latchedX[3];
    i64 latchedY[3];
    
    // Control port bits
    u8 controlPort;
    
    
    //
    // Initializing
    //
    
public:
    
    Mouse1350(C64 &ref) : SubComponent(ref) { }
    const Descriptions &getDescriptions() const override { return descriptions; }

    
    //
    // Methods from CoreObject
    //

private:
    
    const char *getDescription() const override { return "Mouse1350"; }
    
    
    //
    // Methods from CoreComponent
    //

public:

    Mouse1350& operator= (const Mouse1350& other) { return *this; }
    template <class T> void serialize(T& worker) { } SERIALIZERS(serialize);
    void _reset(bool hard) override;


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
    
    // Returns the control port bits triggered by the mouse
    u8 readControlPort() const;
    
    // Execution function (Translates movement deltas into joystick events)
    void execute(i64 targetX, i64 targetY);
};

}
