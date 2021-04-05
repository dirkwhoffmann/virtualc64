// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "PortTypes.h"
#include "C64Component.h"

class Joystick : public C64Component {
    
    // Reference to the control port this device belongs to
    ControlPort &port;
  
    // Button state
    bool button = false;
    
    // Horizontal joystick position (-1 = left, 1 = right, 0 = released)
    int axisX = 0;
    
    // Vertical joystick position (-1 = up, 1 = down, 0 = released)
    int axisY = 0;
    
    // Indicates whether multi-shot mode is enabled
    bool autofire = false;
    
    // Number of bullets per gun volley
    int autofireBullets = -3;
    
    // Autofire frequency in Hz
    float autofireFrequency = 2.5;
    
    // Bullet counter used in multi-fire mode
    i64 bulletCounter = 0;
    
    // Next frame to auto-press or auto-release the fire button
    i64 nextAutofireFrame = 0;
    
    
    //
    // Initializing
    //
    
public:
    
    Joystick(C64 &ref, ControlPort& pref) : C64Component(ref), port(pref) { };
    
    const char *getDescription() const override;
    
private:
    
    void _reset() override { RESET_SNAPSHOT_ITEMS }
    
    
    //
    // Analyzing
    //
    
private:
    
    void _dump() const override;

    
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
    usize _load(const u8 *buffer) override { LOAD_SNAPSHOT_ITEMS }
    usize _save(u8 *buffer) override { SAVE_SNAPSHOT_ITEMS }
    usize didLoadFromBuffer(const u8 *buffer) override;
    
    
    //
    // Managing auto-fire
    //
    
public:
    
    // Configures autofire mode
    bool getAutofire() const { return autofire; }
    void setAutofire(bool value);
    
    // Configures the bullets per gun volley (negative value = infinite)
    int getAutofireBullets() const { return autofireBullets; }
    void setAutofireBullets(int value);
    
    // Configures the autofire frequency
    float getAutofireFrequency() const { return autofireFrequency; }
    void setAutofireFrequency(float value) { autofireFrequency = value; }

private:
    
    // Updates variable nextAutofireFrame
    void scheduleNextShot();
    
    
    //
    // Using the device
    //
    
public:

    // Reads the port bits that show up in the CIA's data port registers
    u8 getControlPort() const;
        
    // Triggers a gamepad event
    void trigger(GamePadAction event);

    /* Execution function for this control port. This method needs to be
     * invoked at the end of each frame to make the auto-fire mechanism work.
     */
    void execute();
};
