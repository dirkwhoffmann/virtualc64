// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef CONTROLPORT_H
#define CONTROLPORT_H

#include "HardwareComponent.h"
#include "ControlPortTypes.h"

class ControlPort : public HardwareComponent {

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
    u64 bulletCounter; 

    //! @brief    Next frame to auto-press or auto-release the fire button
    u64 nextAutofireFrame;

public:
    
    //! @brief    Constructor
    ControlPort(int p);
    
    //! @brief    Destructor
    ~ControlPort();
    
    //! @brief    Method from HardwareComponent
    void reset();

    //! @brief    Method from HardwareComponent
    void didLoadFromBuffer(uint8_t **buffer);
    
    //! @brief    Method from HardwareComponent
    void dump();
    
    //! @brief   Returns true if auto-fire mode is enabled.
    bool getAutofire() { return autofire; }

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
