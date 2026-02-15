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

namespace vc64 {

void
ControlPort::execute()
{
    switch (device) {
            
        case ControlPortDevice::MOUSE:    mouse.execute(); break;
        case ControlPortDevice::JOYSTICK: joystick.eofHandler(); break;

        default:
            break;
    }
}

void
ControlPort::updateControlPort()
{
    if (device == ControlPortDevice::MOUSE) mouse.updateControlPort();
}

u8
ControlPort::getControlPort() const
{
    switch (device) {
            
        case ControlPortDevice::JOYSTICK: return joystick.getControlPort();
        case ControlPortDevice::MOUSE: return mouse.getControlPort();

        default:
            return 0xFF;
    }
}

void
ControlPort::updatePotX()
{
    switch (device) {

        case ControlPortDevice::MOUSE: mouse.updatePotX(); break;

        default:
            break;
    }
}

void
ControlPort::updatePotY()
{
    switch (device) {

        case ControlPortDevice::MOUSE: mouse.updatePotY(); break;

        default:
            break;
    }
}

u8
ControlPort::readPotX() const
{
    switch (device) {

        case ControlPortDevice::MOUSE: return mouse.readPotX();

        default:
            return 0xFF;
    }
}

u8
ControlPort::readPotY() const
{
    switch (device) {

        case ControlPortDevice::MOUSE: return mouse.readPotY();

        default:
            return 0xFF;
    }
}

void
ControlPort::processCommand(const Command &cmd)
{
    switch (cmd.type) {

        case Cmd::MOUSE_MOVE_ABS:    mouse.setXY(cmd.coord.x, cmd.coord.y); break;
        case Cmd::MOUSE_MOVE_REL:    mouse.setDxDy(cmd.coord.x, cmd.coord.y); break;
        case Cmd::MOUSE_BUTTON:       mouse.trigger(cmd.action.action); break;
        case Cmd::JOY_EVENT:         joystick.trigger(cmd.action.action); break;

        default:
            fatalError;
    }
}

}
