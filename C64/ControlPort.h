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
    
    //! @brief    True if multi-shot mode in enabled
    bool autofire;

    //! @brief    Number of bullets per gun volley
    int autofireBullets;

    //! @brief    Autofire frequency in Hz
    float autofireFrequency;

    //! @brief    Bullet counter used in multi-fire mode
    uint64_t bulletCounter; 

    //! @brief    Next frame to auto-press or auto-release the fire button
    uint64_t nextAutofireFrame;

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
    
    //! @brief   Returns true if auto-fire mode is enabled.
    float getAutofire() { return autofire; }

    //! @brief   Enables or disables autofire.
    void setAutofire(bool value);

    /*! @brief   Returns the number of bullets per gun volley.
     *  @details A negative value represents infinity.
     */
    int getAutofireBullets() { return autofireBullets; }

    /*! @brief   Sets the number of bullets per gun volley.
     *  @details A negative value represents infinity.
     */
    void setAutofireBullets(int value);

    //! @brief   Returns the autofire frequency.
    float getAutofireFrequency() { return autofireFrequency; }

    //! @brief   Sets the autofire frequency.
    void setAutofireFrequency(float value) { autofireFrequency = value; }

    //! @brief   Updates variable nextAutofireFrame
    void scheduleNextShot();
    
    //! @brief    Execution function for this control port
    /*! @details  This method is invoked at the end of each frame. It is needed
     *            needed to implement the autofire functionality, only.
     */
    void execute();
    
    //! @brief   Triggers a joystick event
    void trigger(JoystickEvent event);
    
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
