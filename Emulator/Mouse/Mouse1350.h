// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef MOUSE1350_H
#define MOUSE1350_H

#include "C64Component.h"

class Mouse1350 : public C64Component {
    
private:
    
    //! @brief    Mouse position
    i64 mouseX;
    i64 mouseY;
    
    //! @brief    Mouse button states
    bool leftButton;
    bool rightButton;
    
    //! @brief    Dividers applied to raw coordinates in setXY()
    int dividerX = 64;
    int dividerY = 64;
    
    //! @brief    Latched mouse positions
    i64 latchedX[3];
    i64 latchedY[3];
    
    //! @brief    Control port bits
    u8 controlPort;
    
public:
    
    //! @brief   Constructor
    Mouse1350(C64 &ref);
    
    //! @brief   Destructor
    ~Mouse1350();
    
    //! @brief   Methods from HardwareComponent class
    void reset();
        
    //! @brief   Updates the button state
    void setLeftMouseButton(bool value) { leftButton = value; }
    void setRightMouseButton(bool value) { rightButton = value; }

    //! @brief   Returns the pot X bits as set by the mouse
    u8 readPotX();
    
    //! @brief   Returns the pot Y bits as set by the mouse
    u8 readPotY();
    
    //! @brief   Returns the control port bits triggered by the mouse
    u8 readControlPort();
    
    /*! @brief   Execution function
     *  @details Translates movement deltas into joystick events.
     */
    void execute(i64 targetX, i64 targetY);
};

#endif
