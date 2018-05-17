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
#include "Mouse.h"

class NeosMouse : public VirtualComponent, public Mouse {
    
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
    
    //! @brief    Latched horizontal mouse position
    int64_t latchedX;
    
    //! @brief    Latched vertical mouse position
    int64_t latchedY;
    
    //! @brief    The least signifanct value is transmitted to the C64
    int8_t deltaX;
    
    //! @brief    The least signifanct value is transmitted to the C64
    int8_t deltaY;
    
public:
    
    //! @brief    Constructor
    NeosMouse();
    
    //! @brief    Destructor
    ~NeosMouse();
    
    //! @brief    Method from VirtualComponent
    void reset();
    
    //! @brief   Returns the control port bits triggered by the mouse
    uint8_t readControlPort();
    
    //! @brief    Triggers a state change (rising edge on control port line)
    void risingStrobe(int portNr);
    
    //! @brief    Triggers a state change (falling edge on control port line)
    void fallingStrobe(int portNr);
    
private:
    
    //! @brief  Latches the current mouse position and computed deltas
    void latchPosition();
};

#endif
