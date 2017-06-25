/*!
 * @header      Joystick.h
 * @author      Original code by Benjamin Klein, rewritten by Dirk W. Hoffmann
 * @copyright   All rights reserved.
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


#ifndef JOYSTICK_H
#define JOYSTICK_H

#include "VirtualComponent.h"
#include "C64_defs.h"

class Joystick : public VirtualComponent {

private:
    
    //! @brief    True, if button is pressed
    bool button;

    //! @brief    Horizontal joystick position
    JoystickDirection axisX;

    //! @brief    Vertical joystick position
    JoystickDirection axisY;
    
public:

    //! @brief    Constructor
    Joystick();
    
    //! @brief    Destructor
    ~Joystick();
    
    //! @brief    Restores the initial state
    void reset();

    //! @brief    Prints debugging information
    void dumpState();

    inline bool getButton() { return button; }
    inline JoystickDirection getAxisX() { return axisX; }
    inline JoystickDirection getAxisY() { return axisY; }
		
    inline void setButton(bool value) { button = value; }
    inline void setXAxis(JoystickDirection state) { axisX = state; }
    inline void setYAxis(JoystickDirection state) { axisY = state; }
    inline void releaseXAxis() { axisX = JOYSTICK_RELEASED; }
    inline void releaseYAxis() { axisY = JOYSTICK_RELEASED; }

};

#endif
