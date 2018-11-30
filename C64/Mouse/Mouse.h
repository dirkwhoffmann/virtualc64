/*!
 * @header      Mouse.h
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

#ifndef MOUSE_H
#define MOUSE_H

#include "Mouse_types.h"
#include "Mouse1350.h"
#include "Mouse1351.h"
#include "NeosMouse.h"

//! @brief An external mouse plugged into the control port
class Mouse : public VirtualComponent {
    
    //! @brief    Emulated mouse model
    MouseModel model = MOUSE1350;
    
    //! @brief    A Commdore 1350 (digital) mouse
    Mouse1350 mouse1350;

    //! @brief    A Commdore 1351 (analog) mouse
    Mouse1351 mouse1351;

    //! @brief    A Neos (analog) mouse
    NeosMouse mouseNeos;

    /*! @brief    Target mouse position
     *  @details  In order to achieve a smooth mouse movement, a new mouse
     *            coordinate is not written directly into mouseX and mouseY.
     *            Instead, these variables are set. In execute(), mouseX and
     *            mouseY are shifted smoothly towards the target positions.
     */
    int64_t targetX;
    int64_t targetY;
    
public:
    
    //! @brief   Constructor
    Mouse();
    
    //! @brief   Destructor
    ~Mouse();
    
    //! @brief   Reset
    void reset();

    //! @brief   Returns the model of this mouse
    MouseModel mouseModel() { return model; }

    //! @brief   Sets the emulated mouse model
    void setModel(MouseModel model) { this->model = model; }

    //! @brief   Updates the mouse coordinates
    void setXY(int64_t x, int64_t y) { targetX = x; targetY = y; }

    //! @brief   Updates the button states
    void setLeftMouseButton(bool value);
    void setRightMouseButton(bool value);
    
    //! @brief    Triggers a state change (Neos mouse only)
    void risingStrobe(int portNr) { mouseNeos.risingStrobe(portNr); }
    
    //! @brief    Triggers a state change (Neos mouse only)
    void fallingStrobe(int portNr) { mouseNeos.fallingStrobe(portNr); }
    
    //! @brief   Returns the pot X bits as set by the mouse
    uint8_t readPotX();

    //! @brief   Returns the pot Y bits as set by the mouse
    uint8_t readPotY();

    //! @brief   Returns the control port bits as set by the mouse
    uint8_t readControlPort();

    //! @brief   Execution function
    void execute();
};

#endif
