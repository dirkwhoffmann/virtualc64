/*!
 * @header      NeosMouse.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   2018 Dirk W. Hoffmann
 */
/*              This program is free software; you can redistribute it and/or modify
 *              it under the terms of the GNU General Public License as published by
 *              the Free Software Foundation; either version 2 of the License, or
 *              (at your option) any later version.
 *
 *              This program is distributed in the hope that it will be useful,
 *              but WITHOUT ANY WARRANTY; without even the implied warranty of
 *              MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *              GNU General Public License for more details.
 *
 *              You should have received a copy of the GNU General Public License
 *              along with this program; if not, write to the Free Software
 *              Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef NEOSMOUSE_H
#define NEOSMOUSE_H

#include "VirtualComponent.h"

class NeosMouse : public VirtualComponent {
    
private:
        
    //! @brief    Mouse state
    /*! @details  When the mouse switches to state 0, the current mouse
     *            position is latched and the deltaX and deltaY are computed.
     *            After that, the mouse cycles through the other states and
     *            writes the delta values onto the control port, nibble by nibble.
     */
    uint8_t state;

    //! @brief    CPU cycle of the most recent trigger event
    uint64_t triggerCycle;
    
    //! @brief    Horizontal mouse position
    int64_t mouseX;
    
    //! @brief    Vertical mouse position
    int64_t mouseY;
    
    //! @brief    Latched horizontal mouse position
    int64_t latchedX;
    
    //! @brief    Latched vertical mouse position
    int64_t latchedY;
    
    //! @brief    The least signifanct value is transmitted to the C64
    int8_t deltaX;
    
    //! @brief    The least signifanct value is transmitted to the C64
    int8_t deltaY;
    
    //! @brief    Target vertical mouse position
    /*! @details  In order to achieve a smooth mouse movement, a new horizontal
     *            mouse coordinate is not written directly into mouseX.
     *            Instead, this variable is set. In execute(), mouseX is shifted
     *            smoothly towards the target position.
     */
    int64_t targetX;
    
    //! @brief    Target vertical mouse position
    /*! @details  In order to achieve a smooth mouse movement, a new vertical
     *            mouse coordinate is not written directly into mouseY.
     *            Instead, this variable is set. In execute(), mouseY is shifted
     *            smoothly towards the target position.
     */
    int64_t targetY;

    //! @brief    Indicates if left button is pressed
    bool leftButton;

public:
    
    //! @brief    Constructor
    NeosMouse();
    
    //! @brief    Destructor
    ~NeosMouse();
    
    //! @brief    Method from VirtualComponent
    void reset();

    //! @brief   Updates the mouse coordinates
    //! @details Coordinates must range from 0.0 to 1.0
    void setXY(int64_t x, int64_t y);
    
    //! @brief   Pushes or releases the left mouse button
    void setLeftButton(bool pressed);
    
    //! @brief   Pushes or releases the rigt mouse button
    void setRightButton(bool pressed);
    
    //! @brief    Triggers a state change (rising edge on control port line)
    void risingStrobe(int portNr);
    
    //! @brief    Triggers a state change (falling edge on control port line)
    void fallingStrobe(int portNr);
    
    //! @brief   Returns the control port bits triggered by the mouse
    uint8_t readControlPort();
    
    //! @brief   Execution function (called once for each frame)
    /*! @details Shifts mouseX, mouseY smoothly towards mouseTargetX, mouseTargetY
     */
    void execute();
    
private:
    
    //! @brief  Latches the current mouse position and computed deltas
    void latchPosition();
    
};

#endif
