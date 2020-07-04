// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef MOUSE_H
#define MOUSE_H

#include "C64Component.h"
#include "MouseTypes.h"
#include "Mouse1350.h"
#include "Mouse1351.h"
#include "NeosMouse.h"

//! @brief An external mouse plugged into the control port
class Mouse : public C64Component {
    
    //! @brief    A Commdore 1350 (digital) mouse
    Mouse1350 mouse1350 = Mouse1350(vc64);
    
    //! @brief    A Commdore 1351 (analog) mouse
    Mouse1351 mouse1351 = Mouse1351(vc64);
    
    //! @brief    A Neos (analog) mouse
    NeosMouse mouseNeos = NeosMouse(vc64);
    
    //! @brief    Emulated mouse model
    MouseModel model = MOUSE1350;
    
    /*! @brief    The port the mouse is connected to
     *  @details  0 = unconnected, 1 = port 1, 2 = port 2
     */
    unsigned port = 0;

    /*! @brief    Target mouse position
     *  @details  In order to achieve a smooth mouse movement, a new mouse
     *            coordinate is not written directly into mouseX and mouseY.
     *            Instead, these variables are set. In execute(), mouseX and
     *            mouseY are shifted smoothly towards the target positions.
     */
    i64 targetX;
    i64 targetY;
    
public:
    
    //! @brief   Constructor
    Mouse(C64 &ref);
    
    //! @brief   Destructor
    ~Mouse();
    
    //! @brief   Reset
    void reset();

    //! @brief   Returns the model of this mouse.
    MouseModel getModel() { return model; }

    //! @brief   Sets the emulated mouse model.
    void setModel(MouseModel model);

    //! @brief   Returns the port the mouse is connected to (0 = unconnected).
    unsigned getPort() { return port; }
    
    //! @brief    Connects the mouse to the specified port.
    void connectMouse(unsigned port);

    //! @brief    Disconnects the mouse.
    void disconnectMouse() { connectMouse(0); }

    //! @brief   Emulates a mouse movement event.
    void setXY(i64 x, i64 y);

    //! @brief   Emulates a mouse button event.
    void setLeftButton(bool value);
    void setRightButton(bool value);
    
    //! @brief    Triggers a state change (Neos mouse only).
    void risingStrobe(int portNr) {
        mouseNeos.risingStrobe(portNr, targetX, targetY); }
    
    //! @brief    Triggers a state change (Neos mouse only).
    void fallingStrobe(int portNr) {
        mouseNeos.fallingStrobe(portNr, targetX, targetY); }
    
    //! @brief   Returns the pot X bits as set by the mouse.
    u8 readPotX();

    //! @brief   Returns the pot Y bits as set by the mouse.
    u8 readPotY();

    //! @brief   Returns the control port bits as set by the mouse.
    u8 readControlPort(unsigned portNr);

    //! @brief   Performs periodic actions for this device.
    void execute();
};

#endif
