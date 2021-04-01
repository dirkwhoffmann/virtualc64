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
#include "Joystick.h"
#include "Mouse.h"

class ControlPort : public C64Component {

    friend class Mouse;
    friend class Joystick;
    
    // The represented control port
    PortId nr;
        
    // The connected device
    ControlPortDevice device = CPDEVICE_NONE;


    //
    // Sub components
    //

public:
    
    Mouse mouse = Mouse(c64, *this);
    Joystick joystick = Joystick(c64, *this);

        
    //
    // Initializing
    //
    
public:
 
    ControlPort(C64 &ref, PortId id);
    const char *getDescription() const override { return "ControlPort"; }

private:
    
    void _reset() override { RESET_SNAPSHOT_ITEMS };
    

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
    
    
    //
    // Emulating
    //
    
public:
    
    // Invoked at the end of each frame
    void execute();
        
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
};
