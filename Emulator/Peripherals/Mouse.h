// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "C64Component.h"
#include "Mouse1350.h"
#include "Mouse1351.h"
#include "NeosMouse.h"

class Mouse : public C64Component {
    
    // Reference to the control port this device belongs to
    ControlPort &port;

    // Current configuration
    MouseConfig config;

    
    //
    // Sub components
    //
    
private:
    
    // A Commdore 1350 (digital) mouse
    Mouse1350 mouse1350 = Mouse1350(c64);
    
    // A Commdore 1351 (analog) mouse
    Mouse1351 mouse1351 = Mouse1351(c64);
    
    // A Neos (analog) mouse
    NeosMouse mouseNeos = NeosMouse(c64);
        
    /* Target mouse position. In order to achieve a smooth mouse movement, a
     * new mouse coordinate is not written directly into mouseX and mouseY.
     * Instead, these variables are set. In execute(), mouseX and mouseY are
     * shifted smoothly towards the target positions.
     */
    i64 targetX;
    i64 targetY;
  
    
    //
    // Initializing
    //
    
public:
    
    Mouse(C64 &ref, ControlPort& pref);
    const char *getDescription() const override { return "Mouse"; }
    
private:
    
    void _reset() override;

    
    //
    // Analyzing
    //
    
private:
    
    void _dump() const override;
    
    
    //
    // Configuring
    //
    
public:
    
    MouseConfig getConfig() const { return config; }
    
    MouseModel getModel() { return config.model; }
    void setModel(MouseModel model);

    
    //
    // Serializing
    //
    
private:
    
    template <class T>
    void applyToPersistentItems(T& worker)
    {
    }
    
    template <class T>
    void applyToResetItems(T& worker)
    {
    }
    
    usize _size() override { COMPUTE_SNAPSHOT_SIZE }
    usize _load(u8 *buffer) override { LOAD_SNAPSHOT_ITEMS }
    usize _save(u8 *buffer) override { SAVE_SNAPSHOT_ITEMS }
    
    
    //
    // Accessing
    //
    
public:
    
    // Emulates a mouse movement event
    void setXY(i64 x, i64 y);

    // Emulates a mouse button event
    void setLeftButton(bool value);
    void setRightButton(bool value);
    
    // Triggers a gamepad event
    void trigger(GamePadAction event);
    
    // Triggers a state change (Neos mouse only)
    void risingStrobe();
    void fallingStrobe();
    
    // Updates the control port bits (must be called before reading)
    void updateControlPort();
    
    // Returns the control port bits as set by the mouse
    u8 getControlPort() const;

    // Updates the pot bits (must be called before reading)
    void updatePotX();
    void updatePotY();

    // Reads the pot bits that show up in the SID registers
    u8 readPotX() const;
    u8 readPotY() const;

    // Performs periodic actions for this device
    void execute();
};
