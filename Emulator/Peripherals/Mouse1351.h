// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "C64Component.h"

class Mouse1351 : public C64Component {
    
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
        
    Mouse1351(C64 &ref) : C64Component(ref) { }
    const char *getDescription() const override { return "Mouse1351"; }

private:
    
    void _reset() override;
    
    
    //
    // Serializing
    //
    
private:
    
    template <class T>
    void applyToPersistentItems(T& worker)
    {
    }
    
    template <class T>
    void applyToResetItems(T& worker)
    {
    }
    
    usize _size() override { COMPUTE_SNAPSHOT_SIZE }
    usize _load(const u8 *buffer) override { LOAD_SNAPSHOT_ITEMS }
    usize _save(u8 *buffer) override { SAVE_SNAPSHOT_ITEMS }
    
    
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
    u8 mouseXBits() const { return (mouseX & 0x3F) << 1; }
    u8 mouseYBits() const { return (mouseY & 0x3F) << 1; }
};
