/*!
 * @header      Mouse1351.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   Dirk W. Hoffmann. All rights reserved.
 */
/* This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef MOUSE1351_H
#define MOUSE1351_H

#include "VirtualComponent.h"
#include "Mouse.h"

class Mouse1351 : public VirtualComponent, public Mouse {
    
    //! @brief    Mouse movement in pixels per execution step
    int64_t shiftX = 31;
    int64_t shiftY = 31;
    
public:
    
    //! @brief    Constructor
    Mouse1351();
    
    //! @brief   Destructor
    ~Mouse1351();
    
    //! @brief   Methods from VirtualComponent class
    void reset();
    
    //! @brief   Methods from Mouse class
    MouseModel mouseModel() { return MOUSE1351; }
    
    //! @brief   Updates the mouse coordinates
    void setXY(int64_t x, int64_t y);
    
    //! @brief   Returns the control port bits triggered by the mouse
    uint8_t readControlPort();
    
    /*! @brief   Execution function
     *  @details Shifts mouseX and mouseY smoothly towards targetX and targetX.
     */
    void execute();
    
    //! @brief   Returns the mouse X bits as they show up in the SID register
    uint8_t mouseXBits() { return (mouseX & 0x3F) << 1; }

    //! @brief   Returns the mouse Y bits as they show up in the SID register
    uint8_t mouseYBits() { return (mouseY & 0x3F) << 1; }    
};

#endif
