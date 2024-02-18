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

class Mouse1351 : public SubComponent {
    
    // Mouse position
    i64 mouseX;
    i64 mouseY;
    
    // Mouse button states
    bool leftButton;
    bool rightButton;
    
    // Dividers applied to raw coordinates in setXY()
    int dividerX = 256;
    int dividerY = 256;

    // Mouse movement in pixels per execution step
    i64 shiftX = 31;
    i64 shiftY = 31;
    
    //
    // Initializing
    //
    
public:

    Mouse1351(C64 &ref) : SubComponent(ref) { }
    
    
    //
    // Methods from CoreComponent
    //

public:
    
    const char *getDescription() const override { return "Mouse1351"; }

    
    //
    // Methods from CoreComponent
    //

public:

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
    
    // Execution function (Smoothly moves the mouse to the target position)
    void executeX(i64 targetX);
    void executeY(i64 targetY);

    // Returns the mouse bits as they show up in the SID register
    u8 mouseXBits() const { return (u8)((mouseX & 0x3F) << 1); }
    u8 mouseYBits() const { return (u8)((mouseY & 0x3F) << 1); }
};

}
