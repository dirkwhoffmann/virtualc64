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
    
    //! @brief    Indicates whether this object represents control port 1 or control port 2
    /*! @details  Value must be either 1 or 2
     */
    int port;
    
    //! @brief    True, if button is pressed
    bool button;

    //! @brief    Horizontal joystick position
    //@ @details  Valid valued are -1 (LEFT), 1 (RIGHT), or 0 (RELEASED)
    int axisX;

    //! @brief    Vertical joystick position
    //@ details   Valid valued are -1 (UP), 1 (DOWN), or 0 (RELEASED)
    int axisY;
    
public:

    //! @brief    Constructor
    Joystick(int p);
    
    //! @brief    Destructor
    ~Joystick();
    
    //! @brief    Restores the initial state
    void reset();

    //! @brief    Inherited from VirtualComponent
    void loadFromBuffer(uint8_t **buffer);
    
    //! @brief    Prints debugging information
    void dumpState();

    int getPort() { return port; }
    void setPort(int p) { assert(p == 1 || p == 2); port = p; }
    
    //! @brief   Trigger a joystick event
    void trigger(JoystickEvent event);
    
    
    // DEPRECATED
    inline bool getButton() { return button; }
    inline void setButton(bool pressed) { button = pressed; }
    inline void pressButton() { setButton(true); }
    inline void releaseButton() { setButton(false); }
    inline bool isPulledUp() { return axisY == -1; }
    inline bool isPulledDown() { return axisY == 1; }
    inline bool isPulledLeft() { return axisX == -1; }
    inline bool isPulledRight() { return axisX == 1; }
    inline void setXAxis(int value) { axisX = value; }
    inline void setYAxis(int value) { axisY = value; }
    inline void pullUp() { setYAxis(-1); }
    inline void pullDown() { setYAxis(1); }
    inline void pullLeft() { setXAxis(-1); }
    inline void pullRight() { setXAxis(1); }
    inline void releaseXAxis() { setXAxis(0); }
    inline void releaseYAxis() { setYAxis(0); }
    inline void releaseAxes() { releaseXAxis(); releaseYAxis(); }
};

#endif
