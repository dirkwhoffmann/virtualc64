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
#include "Mouse1350.h"
#include "Mouse1351.h"
#include "NeosMouse.h"

class Mouse : public C64Component {
    
    // Current configuration
    MouseConfig config;

    //
    // Sub components
    //
    
private:
    
    // A Commdore 1350 (digital) mouse
    Mouse1350 mouse1350 = Mouse1350(vc64);
    
    // A Commdore 1351 (analog) mouse
    Mouse1351 mouse1351 = Mouse1351(vc64);
    
    // A Neos (analog) mouse
    NeosMouse mouseNeos = NeosMouse(vc64);
    
    // Emulated mouse model
    // MouseModel model = MOUSE1350;
    
    // The port the mouse is connected to (0 = unconnected)
    unsigned port = 0;

    /* Target mouse position
     * In order to achieve a smooth mouse movement, a new mouse coordinate is
     * not written directly into mouseX and mouseY. Instead, these variables
     * are set. In execute(), mouseX and mouseY are shifted smoothly towards
     * the target positions.
     */
    i64 targetX;
    i64 targetY;
  
    
    //
    // Constructing and serializing
    //
    
public:
    
    Mouse(C64 &ref);
    
    //
    // Configuring
    //
    
    MouseConfig getConfig() { return config; }
    
    MouseModel getModel() { return config.model; }
    void setModel(MouseModel model);

    
    //
    // Methods from HardwareComponent
    //
    
private:
    
    void _reset() override;

    
    //
    // Using the device
    //
    
public:
    
    // Returns the port the mouse is connected to (0 = unconnected)
    unsigned getPort() { return port; }
    
    // Connects the mouse to the specified port
    void connectMouse(unsigned port);
    void disconnectMouse() { connectMouse(0); }

    // Emulates a mouse movement event
    void setXY(i64 x, i64 y);

    // Emulates a mouse button event
    void setLeftButton(bool value);
    void setRightButton(bool value);
    
    // Triggers a state change (Neos mouse only)
    void risingStrobe(int portNr);
    void fallingStrobe(int portNr);
    
    // Returns the pot bits as set by the mouse
    u8 readPotX();
    u8 readPotY();

    // Returns the control port bits as set by the mouse
    u8 readControlPort(unsigned portNr);

    // Performs periodic actions for this device
    void execute();
};

#endif
