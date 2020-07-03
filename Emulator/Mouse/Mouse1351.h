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

#include "HardwareComponent.h"

class Mouse1351 : public HardwareComponent {
    
    //! @brief    Mouse position
    i64 mouseX;
    i64 mouseY;
    
    //! @brief    Mouse button states
    bool leftButton;
    bool rightButton;
    
    //! @brief    Dividers applied to raw coordinates in setXY()
    int dividerX = 256;
    int dividerY = 256;
        
    //! @brief    Mouse movement in pixels per execution step
    i64 shiftX = 31;
    i64 shiftY = 31;
    
public:
    
    //! @brief    Constructor
    Mouse1351();
    
    //! @brief   Destructor
    ~Mouse1351();
    
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
     *  @details Shifts mouseX and mouseY smoothly towards targetX and targetX.
     */
    void executeX(i64 targetX);
    void executeY(i64 targetY);

    //! @brief   Returns the mouse X bits as they show up in the SID register
    u8 mouseXBits() { return (mouseX & 0x3F) << 1; }

    //! @brief   Returns the mouse Y bits as they show up in the SID register
    u8 mouseYBits() { return (mouseY & 0x3F) << 1; }    
};

#endif
