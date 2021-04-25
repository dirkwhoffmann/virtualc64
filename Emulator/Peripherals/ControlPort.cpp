// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------
 
#include "config.h"
#include "ControlPort.h"
#include "C64.h"
#include "IO.h"

ControlPort::ControlPort(C64 &ref, PortId id) : C64Component(ref), nr(id)
{
    assert_enum(PortId, id);
    
    subComponents = std::vector<HardwareComponent *> {
        
        &mouse,
        &joystick
    };
}

void
ControlPort::_dump(dump::Category category, std::ostream& os) const
{
    using namespace util;
    
    if (category & dump::State) {
        
        os << tab("Nr");
        os << dec(nr) << std::endl;
        os << tab("Device");
        os << ControlPortDeviceEnum::key(device) << std::endl;
    }
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
