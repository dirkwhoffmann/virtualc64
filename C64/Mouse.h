/*!
 * @header      Mouse.h
 * @author      Dirk W. Hoffmann
 * @copyright   2018. All rights reserved.
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

#ifndef MOUSE_H
#define MOUSE_H

#include "Mouse_types.h"

//! @brief Base class for all mouse emulations
class Mouse {

public:
    
    //! @brief    Indicates if left button is pressed
    bool leftButton;
    
    //! @brief    Indicates if right button is pressed
    bool rightButton;
    
protected:
    
    //! @brief    Mouse position
    int64_t mouseX;
    int64_t mouseY;
    
    //! @brief    Target mouse position
    /*! @details  In order to achieve a smooth mouse movement, a new mouse
     *            coordinate is not written directly into mouseX and mouseY.
     *            Instead, these variables are set. In execute(), mouseX and
     *            mouseY are shifted smoothly towards the target positions.
     */
    int64_t targetX;
    int64_t targetY;
    
    //! @brief    Shift deltas for each execution step
    int64_t shiftX;
    int64_t shiftY;
    
    //! @brief    Scaling dividers applied to raw coordinates in setXY()
    int dividerX;
    int dividerY;

public:
    
    //! @brief   Constructor
    Mouse();
    
    //! @brief   Destructor
    ~Mouse();
    
    //! @brief   Reset
    void reset();

    //! @brief   Returns the model of this mouse
    virtual MouseModel mouseModel() = 0;
    
    //! @brief   Updates the mouse coordinates
    void setXY(int64_t x, int64_t y);
    
    //! @brief   Returns the control port bits triggered by the mouse
    virtual uint8_t readControlPort() = 0;
    
    //! @brief   Execution function
    /*! @details Shifts mouseX and mouseY smoothly towards targetX and targetX.
     */
    virtual void execute();
};

#endif
