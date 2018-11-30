/*!
 * @header      Mouse1350.h
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

#ifndef MOUSE1350_H
#define MOUSE1350_H

#include "VirtualComponent.h"
#include "Mouse.h"

class Mouse1350 : public VirtualComponent, public Mouse {
    
private:
    
    //! @brief    Control port bits
    uint8_t controlPort;
    
    //! @brief    Latched mouse positions
    int64_t latchedX[3];
    int64_t latchedY[3];
    
public:
    
    //! @brief   Constructor
    Mouse1350();
    
    //! @brief   Destructor
    ~Mouse1350();
    
    //! @brief   Methods from VirtualComponent class
    void reset();
    
    //! @brief   Methods from Mouse class
    MouseModel mouseModel() { return MOUSE1350; }
    
    //! @brief   Updates the mouse coordinates
    void setXY(int64_t x, int64_t y);
    
    //! @brief   Returns the control port bits triggered by the mouse
    uint8_t readControlPort();
    
    /*! @brief   Execution function
     *  @details Translates movement deltas into joystick events.
     */
    void execute();
};

#endif
