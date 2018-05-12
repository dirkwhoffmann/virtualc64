/*!
 * @header      ControlPort.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   2018 Dirk W. Hoffmann
 */
/*              This program is free software; you can redistribute it and/or modify
 *              it under the terms of the GNU General Public License as published by
 *              the Free Software Foundation; either version 2 of the License, or
 *              (at your option) any later version.
 *
 *              This program is distributed in the hope that it will be useful,
 *              but WITHOUT ANY WARRANTY; without even the implied warranty of
 *              MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *              GNU General Public License for more details.
 *
 *              You should have received a copy of the GNU General Public License
 *              along with this program; if not, write to the Free Software
 *              Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
 
#include "C64.h"

ControlPort::ControlPort(int portNr) {

    assert(portNr == 1 || portNr == 2);
    
    nr = portNr;
    setDescription("ControlPort");
    debug(3, "    Creating ControlPort %d at address %p...\n", nr, this);
    
    // Register snapshot items
    SnapshotItem items[] = {
        { &mouseX,          sizeof(mouseX),         CLEAR_ON_RESET },
        { &mouseTargetX,    sizeof(mouseTargetX),   CLEAR_ON_RESET },
        { &mouseY,          sizeof(mouseY),         CLEAR_ON_RESET },
        { &mouseTargetY,    sizeof(mouseTargetY),   CLEAR_ON_RESET },
        { NULL,             0,                      0 }};
    
    registerSnapshotItems(items, sizeof(items));
}

ControlPort::~ControlPort()
{
}

void
ControlPort::reset()
{
    VirtualComponent::reset();

    button = false;
    axisX = 0;
    axisY = 0;
    mouseX = mouseTargetX = 0x7F;
    mouseY = mouseTargetY = 0x7F;
}

void
ControlPort::loadFromBuffer(uint8_t **buffer)
{
    VirtualComponent::loadFromBuffer(buffer);
    
    // Discard any active joystick movements
    button = false;
    axisX = 0;
    axisY = 0;
}

void
ControlPort::dumpState()
{
    msg("ControlPort port %d\n", nr);
    msg("------------------\n");
    msg("Button:  %s AxisX: %d AxisY: %d\n", button ? "YES" : "NO", axisX, axisY);
    msg("Bitmask: %02X\n", bitmask());
}

void
ControlPort::trigger(JoystickEvent event)
{
    switch (event) {
            
        case PULL_UP:
            axisY = -1;
            break;
        case PULL_DOWN:
            axisY = 1;
            break;
        case PULL_LEFT:
            axisX = -1;
            break;
        case PULL_RIGHT:
            axisX = 1;
            break;
        case PRESS_FIRE:
            button = true;
            break;
        case RELEASE_X:
            axisX = 0;
            break;
        case RELEASE_Y:
            axisY = 0;
            break;
        case RELEASE_XY:
            axisX = 0;
            axisY = 0;
            break;
        case RELEASE_FIRE:
            button = false;
            break;
        default:
            assert(0);
    }
}

uint8_t
ControlPort::bitmask() {
    
    uint8_t result = 0xFF;
    
    if (axisY == -1) CLR_BIT(result, 0);
    if (axisY ==  1) CLR_BIT(result, 1);
    if (axisX == -1) CLR_BIT(result, 2);
    if (axisX ==  1) CLR_BIT(result, 3);
    if (button)      CLR_BIT(result, 4);
    
    return result;
}

void
ControlPort::execute()
{
    if (mouseX == mouseTargetX && mouseY == mouseTargetY)
        return;
    
    if (mouseTargetX < mouseX) mouseX -= MIN(mouseX - mouseTargetX, 31);
    else if (mouseTargetX > mouseX) mouseX += MIN(mouseTargetX - mouseX, 31);
    if (mouseTargetY < mouseY) mouseY -= MIN(mouseY - mouseTargetY, 31);
    else if (mouseTargetY > mouseY) mouseY += MIN(mouseTargetY - mouseY, 31);
    
    // Let the new mouse coordinate show up in the SID register
    c64->sid.potX = ((mouseX & 0x3F) << 1) | 0x80;
    c64->sid.potY = ((mouseY & 0x3F) << 1) | 0x80;
}

