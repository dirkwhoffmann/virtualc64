// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"
#include "NeosMouse.h"

NeosMouse::NeosMouse() {
    
    setDescription("NeosMouse");
    debug(3, "    Creating NeosMouse at address %p...\n", this);
}

NeosMouse::~NeosMouse()
{
}

void
NeosMouse::reset()
{
    HardwareComponent::reset();
    
    leftButton = false;
    rightButton = false;
    mouseX = 0;
    mouseY = 0;
    state = 0;
    triggerCycle = 0;
    latchedX = 0;
    latchedY = 0;
    deltaX = 0;
    deltaY = 0;
}

u8
NeosMouse::readPotX()
{
    return rightButton ? 0xFF : 0x00;
}

u8
NeosMouse::readPotY()
{
    return 0xFF;
}

u8
NeosMouse::readControlPort(i64 targetX, i64 targetY)
{
    u8 result = leftButton ? 0xE0 : 0xF0;
    
    // Check for time out
    if (state != 0 && c64->cpu.cycle > (triggerCycle + 232) /* from VICE */) {
        state = 0;
        latchPosition(targetX, targetY);
    }
    
    switch (state) {
            
        case 0: // Transmit X_HIGH
            result |= (((u8)deltaX >> 4) & 0x0F);
            break;
            
        case 1: // Transmit X_LOW
            result |= ((u8)deltaX & 0x0F);
            break;
            
        case 2: // Transmit Y_HIGH
            result |= (((u8)deltaY >> 4) & 0x0F);
            break;
            
        case 3: // Transmit Y_LOW
            result |= ((u8)deltaY & 0x0F);
            break;
            
        default:
            assert(false);
    }
    
    return result;
}

/*
void
NeosMouse::execute(i64 targetX, i64 targetY)
{
    targetX /= dividerX;
    targetY /= dividerY;
    
    // Jump directly to target coordinates if they are more than 8 shifts away.
    if (abs(targetX - mouseX) / 8 > shiftX) mouseX = targetX;
    if (abs(targetY - mouseY) / 8 > shiftY) mouseY = targetY;
    
    // Move mouse coordinates towards target coordinates
    if (targetX < mouseX) mouseX -= MIN(mouseX - targetX, shiftX);
    else if (targetX > mouseX) mouseX += MIN(targetX - mouseX, shiftX);
    if (targetY < mouseY) mouseY -= MIN(mouseY - targetY, shiftY);
    else if (targetY > mouseY) mouseY += MIN(targetY - mouseY, shiftY);
}
*/

void
NeosMouse::risingStrobe(int portNr, i64 targetX, i64 targetY)
{
    // Perform rising edge state changes
    switch (state) {
        case 0:  /* X_HIGH -> X_LOW */
            state = 1;
            break;
            
        case 2: /* Y_HIGH -> Y_LOW */
            state = 3;
            break;
    }
    
    // Remember trigger cycle
    triggerCycle = c64->cpu.cycle;
}

void
NeosMouse::fallingStrobe(int portNr, i64 targetX, i64 targetY)
{
    // Perform falling edge state changes
    switch (state) {
        case 1:  /* X_LOW -> Y_HIGH */
            state = 2;
            break;
            
        case 3: /* Y_LOW -> X_HIGH */
            state = 0;
            latchPosition(targetX, targetY);
            break;
    }
    
    // Remember trigger cycle
    triggerCycle = c64->cpu.cycle;
}

void
NeosMouse::latchPosition(i64 targetX, i64 targetY)
{
    //
    // Shift mouseX and mouseY towards targetX and targetY
    //
    
    targetX /= dividerX;
    targetY /= dividerY;
    
    // Jump directly to target coordinates if they are more than 8 shifts away.
    if (abs(targetX - mouseX) / 8 > shiftX) mouseX = targetX;
    if (abs(targetY - mouseY) / 8 > shiftY) mouseY = targetY;
    
    // Move mouse coordinates towards target coordinates
    if (targetX < mouseX) mouseX -= MIN(mouseX - targetX, shiftX);
    else if (targetX > mouseX) mouseX += MIN(targetX - mouseX, shiftX);
    if (targetY < mouseY) mouseY -= MIN(mouseY - targetY, shiftY);
    else if (targetY > mouseY) mouseY += MIN(targetY - mouseY, shiftY);
    
    //
    // Compute deltas and latch values
    //
    
    i64 dx = MAX(MIN((latchedX - mouseX), 127), -128);
    i64 dy = MAX(MIN((mouseY - latchedY), 127), -128);
    
    deltaX = (u8)dx;
    deltaY = (u8)dy;
    
    latchedX = mouseX;
    latchedY = mouseY;
}

