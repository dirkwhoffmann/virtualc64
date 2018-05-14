/*!
 * @header      NeosMouse.cpp
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

NeosMouse::NeosMouse() {
    
    setDescription("NeosMouse");
    debug(3, "    Creating NeosMouse at address %p...\n", this);
    
    // Register snapshot items
    SnapshotItem items[] = {
        { &state,           sizeof(state),          CLEAR_ON_RESET },
        { &triggerCycle,    sizeof(triggerCycle),   CLEAR_ON_RESET },
        { &mouseX,          sizeof(mouseX),         CLEAR_ON_RESET },
        { &latchedX,        sizeof(latchedX),       CLEAR_ON_RESET },
        { &mouseY,          sizeof(mouseY),         CLEAR_ON_RESET },
        { &latchedY,        sizeof(latchedY),       CLEAR_ON_RESET },
        { &mouseTargetX,    sizeof(mouseTargetX),   CLEAR_ON_RESET },
        { &mouseTargetY,    sizeof(mouseTargetY),   CLEAR_ON_RESET },
        { &deltaX,          sizeof(deltaX),         CLEAR_ON_RESET },
        { &deltaY,          sizeof(deltaY),         CLEAR_ON_RESET },
        { &leftButton,      sizeof(leftButton),     CLEAR_ON_RESET },
        { NULL,             0,                      0 }};
    
    registerSnapshotItems(items, sizeof(items));
}

NeosMouse::~NeosMouse()
{
}

void
NeosMouse::reset()
{
    VirtualComponent::reset();
}

void
NeosMouse::connect(unsigned port)
{
    assert(port == 0 || port == 1 || port == 2);
    
    debug("Connecting NeosMouse to port %d\n", port);
    this->port = port;
    
    if (port == 0) {
        // Disconnection stuff
    }
}

void
NeosMouse::risingStrobe(int portNr)
{
    // Check if mouse is connected to the specified port
    if (port != portNr)
        return;
    
    // Perform rising edge state changes
    switch (state) {
        case 0:  /* X_HIGH -> X_LOW */
            state = 1;
            break;

        case 2: /* Y_HIGH -> Y_LOW */
            state = 3;
            break;
    }
    
    // debug("State change to %d\n", state);

    // Remember trigger cycle
    triggerCycle = c64->cycle;
}

void
NeosMouse::fallingStrobe(int portNr)
{
    // Check if mouse is connected to the specified port
    if (port != portNr)
        return;
    
    // Perform falling edge state changes
    switch (state) {
        case 1:  /* X_LOW -> Y_HIGH */
            state = 2;
            break;
            
        case 3: /* Y_LOW -> X_HIGH */
            state = 0;
            latchPosition();
            break;
    }
    
    // debug("State change to %d\n", state);
    
    // Remember trigger cycle
    triggerCycle = c64->cycle;
}

void
NeosMouse::setXY(int64_t x, int64_t y)
{
    mouseX = x;
    mouseY = y;
    
    // Translate into C64 coordinate system (this is a hack)
    /*
    const double xmax = 190.0; // 320; // 380.0;
    const double ymax = 268.0;
    x = x * xmax;
    y = y * ymax;
    mouseTargetX = (uint32_t)x; // (uint32_t)MIN(MAX(x, 0.0), xmax);
    mouseTargetY = (uint32_t)y; // (uint32_t)MIN(MAX(y, 0.0), ymax);
     */
}

void
NeosMouse::setLeftButton(bool pressed)
{
    leftButton = pressed;
}

void
NeosMouse::setRightButton(bool pressed)
{
    // The right mouse button is connected to the POTX control port pin
    // We don't emulate this button
}

uint8_t
NeosMouse::readControlPort()
{
    uint8_t result = leftButton ? 0xE0 : 0xF0;
    
    // debug("NEOS read at cycle %d\n", c64->cycle);
    
    // Check for time out
    if (state != 0 && c64->cycle > triggerCycle + (2*232) /* from VICE */) {
        // debug("    TIME OUT\n");
        state = 0;
        latchPosition();
    }
    
    // debug("Reading %d %d from port %d (state = %d)\n", deltaX, deltaY, portNr, state);
    
    switch (state) {
            
        case 0: // Transmit X_HIGH
            result |= (((uint8_t)deltaX >> 4) & 0x0F);
            
        case 1: // Transmit X_LOW
            result |= ((uint8_t)deltaX & 0x0F);
            
        case 2: // Transmit Y_HIGH
            result |= (((uint8_t)deltaY >> 4) & 0x0F);
            
        case 3: // Transmit Y_LOW
            result |= ((uint8_t)deltaY & 0x0F);
            
        default:
            assert(false);
    }
    
    return result;
}

void
NeosMouse::execute()
{
    if (mouseX == mouseTargetX && mouseY == mouseTargetY)
        return;
    
    if (mouseTargetX < mouseX) mouseX -= MIN(mouseX - mouseTargetX, 31);
    else if (mouseTargetX > mouseX) mouseX += MIN(mouseTargetX - mouseX, 31);
    if (mouseTargetY < mouseY) mouseY -= MIN(mouseY - mouseTargetY, 31);
    else if (mouseTargetY > mouseY) mouseY += MIN(mouseTargetY - mouseY, 31);
}

void
NeosMouse::latchPosition()
{
    int64_t dx = MAX(MIN((latchedX - mouseX), 127), -128);
    int64_t dy = MAX(MIN((mouseY - latchedY), 127), -128);
    
    deltaX = (uint8_t)dx;
    deltaY = (uint8_t)dy;
    
    //debug("Latching X:%04X(%04X) %d Y:%04X(%04X) %d state = %d\n",
    //       mouseX, latchedX, deltaX,
    //       mouseY, latchedY, deltaY, state);
    
    latchedX = mouseX;
    latchedY = mouseY;
}
