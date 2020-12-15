// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef MOUSE1351_H
#define MOUSE1351_H

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
    const char *getDescription() override { return "Mouse1351"; }

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
    
    size_t _size() override { COMPUTE_SNAPSHOT_SIZE }
    size_t _load(u8 *buffer) override { LOAD_SNAPSHOT_ITEMS }
    size_t _save(u8 *buffer) override { SAVE_SNAPSHOT_ITEMS }
    
    
    //
    // Accessing
    //
    
public:
    
    // Updates the button state
    void setLeftMouseButton(bool value) { leftButton = value; }
    void setRightMouseButton(bool value) { rightButton = value; }
    
    // Returns the pot bits as set by the mouse
    u8 readPotX();
    u8 readPotY();
    
    // Returns the control port bits triggered by the mouse
    u8 readControlPort();
    
    // Execution function (Smoothly moves the mouse to the target position)
    void executeX(i64 targetX);
    void executeY(i64 targetY);

    // Returns the mouse bits as they show up in the SID register
    u8 mouseXBits() { return (mouseX & 0x3F) << 1; }
    u8 mouseYBits() { return (mouseY & 0x3F) << 1; }
};

#endif
