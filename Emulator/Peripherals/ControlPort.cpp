// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------
 
#include "C64.h"

ControlPort::ControlPort(C64 &ref, PortId id) : C64Component(ref), nr(id)
{
    assert_enum(PortId, id);
    
    subComponents = vector<HardwareComponent *> {
        
        &mouse,
        &joystick
    };
}

void
ControlPort::_dump() const
{
}

void
ControlPort::execute()
{
    switch (device) {
            
        case CPDEVICE_JOYSTICK: joystick.execute(); break;
        case CPDEVICE_MOUSE: mouse.execute(); break;
            
        default:
            break;
    }
}

void
ControlPort::updateControlPort()
{
    if (device == CPDEVICE_MOUSE) mouse.updateControlPort();
}

u8
ControlPort::getControlPort() const
{
    switch (device) {
            
        case CPDEVICE_JOYSTICK: return joystick.getControlPort();
        case CPDEVICE_MOUSE: return mouse.getControlPort();
            
        default:
            return 0xFF;
    }
}

void
ControlPort::updatePotX()
{
    if (device == CPDEVICE_MOUSE) mouse.updatePotX();
}

void
ControlPort::updatePotY()
{
    if (device == CPDEVICE_MOUSE) mouse.updatePotY();
}

u8
ControlPort::readPotX() const
{
    if (device == CPDEVICE_MOUSE) return mouse.readPotX();
    
    return 0xFF;
}

u8
ControlPort::readPotY() const
{
    if (device == CPDEVICE_MOUSE) return mouse.readPotY();
    
    return 0xFF;
}
