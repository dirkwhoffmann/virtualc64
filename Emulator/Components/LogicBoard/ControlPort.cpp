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

#include "config.h"
#include "ControlPort.h"
#include "C64.h"
#include "IOUtils.h"

namespace vc64 {

ControlPort::ControlPort(C64 &ref, isize nr) : SubComponent(ref), nr(nr)
{
    assert(nr == PORT_1 || nr == PORT_2);
    
    subComponents = std::vector<CoreComponent *> {
        
        &mouse,
        &joystick
    };
}

void
ControlPort::_dump(Category category, std::ostream& os) const
{
    using namespace util;
    
    if (category == Category::State) {
        
        os << tab("Port Nr");
        os << dec(nr) << std::endl;
        os << tab("Detetected device");
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

void
ControlPort::processCommand(const Cmd &cmd)
{
    switch (cmd.type) {

        case CMD_MOUSE_MOVE_ABS:    mouse.setXY(cmd.coord.x, cmd.coord.y); break;
        case CMD_MOUSE_MOVE_REL:    mouse.setDxDy(cmd.coord.x, cmd.coord.y); break;
        case CMD_MOUSE_EVENT:       mouse.trigger(cmd.action.action); break;
        case CMD_JOY_EVENT:         joystick.trigger(cmd.action.action); break;

        default:
            fatalError;
    }
}

}
