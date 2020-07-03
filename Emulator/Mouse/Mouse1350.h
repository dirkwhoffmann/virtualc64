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

#include "HardwareComponent.h"

class Mouse1350 : public HardwareComponent {
    
private:
    
    //! @brief    Mouse position
    int64_t mouseX;
    int64_t mouseY;
    
    //! @brief    Mouse button states
    bool leftButton;
    bool rightButton;
    
    //! @brief    Dividers applied to raw coordinates in setXY()
    int dividerX = 64;
    int dividerY = 64;
    
    //! @brief    Latched mouse positions
    int64_t latchedX[3];
    int64_t latchedY[3];
    
    //! @brief    Control port bits
    uint8_t controlPort;
    
public:
    
    //! @brief   Constructor
    Mouse1350();
    
    //! @brief   Destructor
    ~Mouse1350();
    
    //! @brief   Methods from HardwareComponent class
    void reset();
        
    //! @brief   Updates the button state
    void setLeftMouseButton(bool value) { leftButton = value; }
    void setRightMouseButton(bool value) { rightButton = value; }

    //! @brief   Returns the pot X bits as set by the mouse
    uint8_t readPotX();
    
    //! @brief   Returns the pot Y bits as set by the mouse
    uint8_t readPotY();
    
    //! @brief   Returns the control port bits triggered by the mouse
    uint8_t readControlPort();
    
    /*! @brief   Execution function
     *  @details Translates movement deltas into joystick events.
     */
    void execute(int64_t targetX, int64_t targetY);
};

#endif
