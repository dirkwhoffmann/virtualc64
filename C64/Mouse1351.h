/*!
 * @header      Mouse1351.h
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

#ifndef MOUSE1351_H
#define MOUSE1351_H

#include "VirtualComponent.h"

class Mouse1351 : public VirtualComponent {
    
private:
    
    //! @brief    Control port this mouse is connected to
    //! @details  0 = unconnected, 1 = Port 1, 2 = Port 2
    uint8_t port;
    
    //! @brief    Horizontal mouse position
    int64_t mouseX;
    
    //! @brief    Vertical mouse position
    int64_t mouseY;
    
    //! @brief    Target mouse X position
    /*! @details  In order to achieve a smooth mouse movement, a new horizontal
     *            mouse coordinate is not written directly into mouseX.
     *            Instead, this variable is set. In execute(), mouseX is shifted
     *            smoothly towards the target position.
     */
    int64_t targetX;
    
    //! @brief    Target mouse Y position
    /*! @details  In order to achieve a smooth mouse movement, a new vertical
     *            mouse coordinate is not written directly into mouseY.
     *            Instead, this variable is set. In execute(), mouseY is shifted
     *            smoothly towards the target position.
     */
    int64_t targetY;
    
    //! @brief    Control port bits
    uint8_t controlPort; 
    
public:
    
    //! @brief    Constructor
    Mouse1351();
    
    //! @brief    Destructor
    ~Mouse1351();
    
    //! @brief    Method from VirtualComponent
    void reset();
    
    //! @brief   Connects the mouse to one of the two control ports
    void connect(unsigned port);

    //! @brief   Disconnects the mouse
    void disconnect() { connect(0); }

    //! @brief   Updates the mouse coordinates
    void setXY(int64_t x, int64_t y);

    //! @brief   Pushes or releases the left mouse button
    void setLeftButton(bool pressed);

    //! @brief   Pushes or releases the rigt mouse button
    void setRightButton(bool pressed);

    //! @brief   Returns the control port bits triggered by the mouse
    uint8_t readControlPort() { return controlPort; }
    
    //! @brief   Execution function (called once for each frame)
    /*! @details Shifts mouseX, mouseY smoothly towards mouseTargetX, mouseTargetY
     */
    void execute();
};


#endif
