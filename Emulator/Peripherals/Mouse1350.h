// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "C64Component.h"

class Mouse1350 : public C64Component {
    
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
    
    Mouse1350(C64 &ref) : C64Component(ref) { }
    const char *getDescription() const override { return "Mouse1350"; }
    
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
    
    isize _size() override { COMPUTE_SNAPSHOT_SIZE }
    isize _load(const u8 *buffer) override { LOAD_SNAPSHOT_ITEMS }
    isize _save(u8 *buffer) override { SAVE_SNAPSHOT_ITEMS }
    
    
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
