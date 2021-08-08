// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "MouseTypes.h"
#include "JoystickTypes.h"
#include "SubComponent.h"
#include "Chrono.h"
#include "Mouse1350.h"
#include "Mouse1351.h"
#include "NeosMouse.h"

class ShakeDetector {
    
    // Horizontal position
    double x = 0.0;
    
    // Moved distance
    double dxsum = 0.0;

    // Direction (1 or -1)
    double dxsign = 1.0;
    
    // Number of turns
    isize dxturns = 0;
    
    // Time stamps
    u64 lastTurn = 0;
    util::Time lastShake;
    
public:
    
    // Feed in new coordinates and checks for a shake
    bool isShakingAbs(double x);
    bool isShakingRel(double dx);
};

class Mouse : public SubComponent {
    
    // Reference to the control port this device belongs to
    ControlPort &port;

    // Current configuration
    MouseConfig config = { };

    
    //
    // Sub components
    //

    // Shake detector
    class ShakeDetector shakeDetector;

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
    double targetX = 0.0;
    double targetY = 0.0;
  
    // Scaling factors applied to the raw mouse coordinates in setXY()
    double scaleX = 1.0;
    double scaleY = 1.0;

    
    //
    // Initializing
    //
    
public:
    
    Mouse(C64 &ref, ControlPort& pref);
    const char *getDescription() const override { return "Mouse"; }
    
private:
    
    void _reset(bool hard) override;

    
    //
    // Configuring
    //
    
public:
    
    const MouseConfig &getConfig() const { return config; }
    void resetConfig() override;

    i64 getConfigItem(Option option) const;
    void setConfigItem(Option option, i64 value) override;
    void setConfigItem(Option option, long id, i64 value) override;
    
private:
    
    void updateScalingFactors();
    
    
    //
    // Analyzing
    //
    
private:
    
    void _dump(dump::Category category, std::ostream& os) const override;


    //
    // Serializing
    //
    
private:
    
    template <class T>
    void applyToPersistentItems(T& worker)
    {
        worker << config.model;
    }
    
    template <class T>
    void applyToResetItems(T& worker, bool hard = true)
    {
    }
    
    isize _size() override { COMPUTE_SNAPSHOT_SIZE }
    isize _load(const u8 *buffer) override { LOAD_SNAPSHOT_ITEMS }
    isize _save(u8 *buffer) override { SAVE_SNAPSHOT_ITEMS }
    
    
    //
    // Accessing
    //
    
public:

    // Runs the shake detector
    bool detectShakeXY(double x, double y);
    bool detectShakeDxDy(double dx, double dy);

    // Emulates a mouse movement
    void setXY(double x, double y);
    void setDxDy(double dx, double dy);

    // Presses or releases a mouse button
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
