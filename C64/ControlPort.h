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
    
    //! @brief    Represented control port (1 or 2)
    int nr;
    
    //! @brief    True, if button is pressed.
    bool button;

    /*! @brief    Horizontal joystick position
     *  @details  Valid valued are -1 (LEFT), 1 (RIGHT), or 0 (RELEASED)
     */
    int axisX;

    /*! @brief    Vertical joystick position
     *  @details   Valid valued are -1 (UP), 1 (DOWN), or 0 (RELEASED)
     */
    int axisY;
    
    //! @brief    True, if autofire is enabled.
    bool autofire;

    //! @brief    Autofire frequency in Hz
    float autofireFrequency;

    
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
    
    //! @brief    Execution function for this control port
    /*! @details  This method is invoked at the end of each frame. It is needed
     *            needed to implement the autofire functionality, only.
     */
    void execute(uint64_t frame);
    
    //! @brief   Triggers a joystick event
    void trigger(JoystickEvent event);

    //! @brief   Enables or disables autofire.
    void setAutofire(bool value);
    
    /*! @brief   Returns the current joystick movement in form a bit mask
     *  @details The bits are in the same order as they show up in the
     *           CIA's data port registers
     */
    uint8_t bitmask();

    //! @brief   Returns the potentiometer X value (analog mouse)
    uint8_t potX();

    //! @brief   Returns the potentiometer Y value (analog mouse)
    uint8_t potY();
};

#endif
