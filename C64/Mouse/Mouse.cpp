/*!
 * @file        Mouse.cpp
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   Dirk W. Hoffmann. All rights reserved.
 */
/* This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "C64.h"

Mouse::Mouse()
{
    setDescription("Mouse");
    debug(3, "Creating %s at address %p\n", getDescription());
    
    // Register sub components
    VirtualComponent *subcomponents[] = { &mouse1350, &mouse1351, &mouseNeos, NULL };
    registerSubComponents(subcomponents, sizeof(subcomponents));
}

Mouse::~Mouse()
{
}

void Mouse::reset()
{
    VirtualComponent::reset();
    targetX = 0;
    targetY = 0;
}
void
Mouse::setModel(MouseModel model)
{
    c64->suspend();
    this->model = model;
    reset();
    c64->resume();
}

void
Mouse::connectMouse(unsigned portNr)
{
    // debug("Connecting mouse to port %d\n", portNr);
    
    assert(portNr <= 2);
    port = portNr;
}

void
Mouse::setXY(int64_t x, int64_t y)
{
    targetX = x;
    targetY = y;
}

void
Mouse::setLeftButton(bool value)
{
    switch(model) {
        case MOUSE1350:
            mouse1350.setLeftMouseButton(value);
            break;
        case MOUSE1351:
            mouse1351.setLeftMouseButton(value);
            break;
        case NEOSMOUSE:
            mouseNeos.setLeftMouseButton(value);
            break;
        default:
            assert(false);
    }
}

void
Mouse::setRightButton(bool value)
{
    switch(model) {
        case MOUSE1350:
            mouse1350.setRightMouseButton(value);
            break;
        case MOUSE1351:
            mouse1351.setRightMouseButton(value);
            break;
        case NEOSMOUSE:
            mouseNeos.setRightMouseButton(value);
            break;
        default:
            assert(false);
    }
}

uint8_t
Mouse::readPotX()
{
    if (port > 0) {
        switch(model) {
            case MOUSE1350:
                return mouse1350.readPotX();
            case MOUSE1351:
                mouse1351.executeX(targetX);
                return mouse1351.readPotX();
            case NEOSMOUSE:
                return mouseNeos.readPotX();
            default:
                assert(false);
        }
    }
    return 0xFF;
}

uint8_t
Mouse::readPotY()
{
    if (port > 0) {
        switch(model) {
            case MOUSE1350:
                return mouse1350.readPotY();
            case MOUSE1351:
                mouse1351.executeY(targetY);
                return mouse1351.readPotY();
            case NEOSMOUSE:
                return mouseNeos.readPotY();
            default:
                assert(false);
        }
    }
    return 0xFF;
}

uint8_t
Mouse::readControlPort(unsigned portNr)
{
    // debug("port = %d portNr = %d\n", port, portNr);
    
    if (port == portNr) {
        switch(model) {
            case MOUSE1350:
                // mouse1350.execute(targetX, targetY);
                return mouse1350.readControlPort();
            case MOUSE1351:
                return mouse1351.readControlPort();
            case NEOSMOUSE:
                return mouseNeos.readControlPort(targetX, targetY);
            default:
                assert(false);
        }
    }
    return 0xFF;
}

void
Mouse::execute()
{
    if (port) {
        switch(model) {
            case MOUSE1350:
                mouse1350.execute(targetX, targetY);
                break;
            case MOUSE1351:
                // Coordinates are updated in readPotX and readPotY
                break;
            case NEOSMOUSE:
                // mouseNeos.execute(targetX, targetY);
                break;
            default:
                assert(false);
        }
    }
}
