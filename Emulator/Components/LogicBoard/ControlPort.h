// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// This FILE is dual-licensed. You are free to choose between:
//
//     - The GNU General Public License v3 (or any later version)
//     - The Mozilla Public License v2
//
// SPDX-License-Identifier: GPL-3.0-or-later OR MPL-2.0
// -----------------------------------------------------------------------------

#pragma once

#include "ControlPortTypes.h"
#include "SubComponent.h"
#include "Joystick.h"
#include "Mouse.h"

namespace vc64 {

class ControlPort : public SubComponent {

    friend class Mouse;
    friend class Joystick;
    
    // The represented control port (1 or 2)
    isize nr;

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

    ControlPort(C64 &ref, isize id);


    //
    // Methods from CoreObject
    //

private:
    
    const char *getDescription() const override { return "ControlPort"; }
    void _dump(Category category, std::ostream& os) const override;

    
    //
    // Methods from CoreComponent
    //

private:
    
    void _reset(bool hard) override { RESET_SNAPSHOT_ITEMS(hard) };

    template <class T> void serialize(T& worker) { }
    isize _size() override { COMPUTE_SNAPSHOT_SIZE }
    u64 _checksum() override { COMPUTE_SNAPSHOT_CHECKSUM }
    isize _load(const u8 *buffer) override { LOAD_SNAPSHOT_ITEMS }
    isize _save(u8 *buffer) override { SAVE_SNAPSHOT_ITEMS }
    
    
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

}
