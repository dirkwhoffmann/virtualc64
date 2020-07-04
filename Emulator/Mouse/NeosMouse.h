// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef NEOSMOUSE_H
#define NEOSMOUSE_H

#include "C64Component.h"

class NeosMouse : public C64Component {
    
    //! @brief    Mouse position
    i64 mouseX;
    i64 mouseY;
    
    //! @brief    Mouse button states
    bool leftButton;
    bool rightButton;
    
    //! @brief    Dividers applied to raw coordinates in setXY()
    int dividerX = 512;
    int dividerY = 256;
    
    //! @brief    Mouse movement in pixels per execution step
    i64 shiftX = 127;
    i64 shiftY = 127;
    
    //! @brief    Mouse state
    /*! @details  When the mouse switches to state 0, the current mouse
     *            position is latched and the deltaX and deltaY are computed.
     *            After that, the mouse cycles through the other states and
     *            writes the delta values onto the control port, nibble by nibble.
     */
    u8 state;

    //! @brief    CPU cycle of the most recent trigger event
    u64 triggerCycle;
    
    //! @brief    Latched horizontal mouse position
    i64 latchedX;
    
    //! @brief    Latched vertical mouse position
    i64 latchedY;
    
    //! @brief    The least signifanct value is transmitted to the C64
    i8 deltaX;
    
    //! @brief    The least signifanct value is transmitted to the C64
    i8 deltaY;
    
public:
    
    //! @brief    Constructor
    NeosMouse(C64 &ref);
    
    //! @brief    Destructor
    ~NeosMouse();
    
    //! @brief    Methods from HardwareComponent class
    void reset();
    
    //! @brief   Updates the button state
    void setLeftMouseButton(bool value) { leftButton = value; }
    void setRightMouseButton(bool value) { rightButton = value; }
    
    //! @brief   Returns the pot X bits as set by the mouse
    u8 readPotX();
    
    //! @brief   Returns the pot Y bits as set by the mouse
    u8 readPotY();
    
    //! @brief   Returns the control port bits triggered by the mouse
    u8 readControlPort(i64 targetX, i64 targetY);
    
    /*! @brief   Execution function
     *  @details Shifts mouseX and mouseY smoothly towards targetX and targetX.
     */
    // void execute(i64 targetX, i64 targetY);
    
    //! @brief    Triggers a state change (rising edge on control port line)
    void risingStrobe(int portNr, i64 targetX, i64 targetY);
    
    //! @brief    Triggers a state change (falling edge on control port line)
    void fallingStrobe(int portNr, i64 targetX, i64 targetY);
    
private:
    
    //! @brief  Latches the current mouse position and computed deltas
    void latchPosition(i64 targetX, i64 targetY);
};

#endif
