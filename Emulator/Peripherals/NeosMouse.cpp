// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "C64.h"
#include "NeosMouse.h"
#include <algorithm>

void
NeosMouse::_reset()
{
    RESET_SNAPSHOT_ITEMS

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
NeosMouse::readPotX() const
{
    return rightButton ? 0xFF : 0x00;
}

u8
NeosMouse::readPotY() const
{
    return 0xFF;
}

void
NeosMouse::updateControlPort(i64 targetX, i64 targetY)
{
    // Check for time out
    if (state != 0 && cpu.cycle > (triggerCycle + 232) /* from VICE */) {
        state = 0;
        latchPosition(targetX, targetY);
    }
}

u8
NeosMouse::readControlPort() const
{
    u8 result = leftButton ? 0xE0 : 0xF0;

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

void
NeosMouse::risingStrobe(i64 targetX, i64 targetY)
{
    // Perform rising edge state changes
    switch (state) {
            
        case 0:  // X_HIGH -> X_LOW
            state = 1;
            break;
            
        case 2:  // Y_HIGH -> Y_LOW
            state = 3;
            break;
    }
    
    // Remember trigger cycle
    triggerCycle = cpu.cycle;
}

void
NeosMouse::fallingStrobe(i64 targetX, i64 targetY)
{
    // Perform falling edge state changes
    switch (state) {
            
        case 1:  // X_LOW -> Y_HIGH
            state = 2;
            break;
            
        case 3:  // Y_LOW -> X_HIGH
            state = 0;
            latchPosition(targetX, targetY);
            break;
    }
    
    // Remember trigger cycle
    triggerCycle = cpu.cycle;
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
    if (targetX < mouseX) mouseX -= std::min(mouseX - targetX, shiftX);
    else if (targetX > mouseX) mouseX += std::min(targetX - mouseX, shiftX);
    if (targetY < mouseY) mouseY -= std::min(mouseY - targetY, shiftY);
    else if (targetY > mouseY) mouseY += std::min(targetY - mouseY, shiftY);
    
    //
    // Compute deltas and latch values
    //
    
    i64 dx = std::clamp(latchedX - mouseX, 127LL, -128LL);
    i64 dy = std::clamp(mouseY - latchedY, 127LL, -128LL);
    
    deltaX = (u8)dx;
    deltaY = (u8)dy;
    
    latchedX = mouseX;
    latchedY = mouseY;
}

