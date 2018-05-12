/*!
 * @header      ControlPort.h
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

#ifndef CONTROLPORT_H
#define CONTROLPORT_H

#include "VirtualComponent.h"
#include "ControlPort_types.h"

class ControlPort : public VirtualComponent {

private:
    
    //! @brief    Indicates whether this object represents control port 1 or control port 2
    /*! @details  Value must be either 1 or 2
     */
    int nr;
    
    //! @brief    True, if button is pressed
    bool button;

    //! @brief    Horizontal joystick position
    //@ @details  Valid valued are -1 (LEFT), 1 (RIGHT), or 0 (RELEASED)
    int axisX;

    //! @brief    Vertical joystick position
    //@ details   Valid valued are -1 (UP), 1 (DOWN), or 0 (RELEASED)
    int axisY;
    
public:
    
    //! @brief    Horizontal mouse coordinate of a connected analog mouse
    //! @details  Set to 0x7F if no mouse is connected.
    uint32_t mouseX;
    
    //! @brief    Vertical mouse coordinate of a connected analog mouse
    //! @details  Set to 0x7F if no mouse is connected.
    uint32_t mouseY;
    
    //! @brief    Target mouse X position
    /*! @details  In order to achieve a smooth mouse movement, a new horizontal
     *            mouse coordinate is not written directly into mouseX.
     *            Instead, this variable is set. In execute(), mouseX is shifted
     *            smoothly towards the target position.
     */
    uint32_t mouseTargetX;
    
    //! @brief    Target mouse Y position
    /*! @details  In order to achieve a smooth mouse movement, a new vertical
     *            mouse coordinate is not written directly into mouseY.
     *            Instead, this variable is set. In execute(), mouseY is shifted
     *            smoothly towards the target position.
     */
    uint32_t mouseTargetY;

public:

    //! @brief    Constructor
    ControlPort(int p);
    
    //! @brief    Destructor
    ~ControlPort();
    
    //! @brief    Method from VirtualComponent
    void reset();

    //! @brief    Method from VirtualComponent
    void loadFromBuffer(uint8_t **buffer);
    
    //! @brief    Method from VirtualComponent
    void dumpState();
    
    //! @brief   Triggers a joystick event
    void trigger(JoystickEvent event);
    
    /*! @brief   Returns the current joystick movement in form a bit mask
     *  @details The bits are in the same order as they show up in the
     *           CIA's data port registers
     */
    uint8_t bitmask();
    
    //! @brief   Execution function (called once for each frame)
    /*! @details Shifts mouseX, mouseY smoothly towards mouseTargetX, mouseTargetY
     */
    void execute();
    
};

#endif
