// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"

VICIIInfo
VICII::getInfo()
{
    VICIIInfo info;
    
    u8 ctrl1 = reg.current.ctrl1;
    u8 ctrl2 = reg.current.ctrl2;

    info.rasterLine = c64.rasterLine;
    info.rasterCycle = c64.rasterCycle;
    info.yCounter = yCounter;
    info.xCounter = xCounter;
    info.vc = vc;
    info.vcBase = vcBase;
    info.rc = rc;
    info.vmli = vmli;
    
    info.ctrl1 = ctrl1;
    info.ctrl2 = ctrl2;
    info.dy = ctrl1 & 0x07;
    info.dx = ctrl2 & 0x07;
    info.denBit = DENbit();
    info.badLine = badLine;
    info.displayState = displayState;
    info.screenGeometry = getScreenGeometry();
    info.frameFF = flipflops.current;
    info.displayMode = (DisplayMode)((ctrl1 & 0x60) | (ctrl2 & 0x10));
    info.borderColor = reg.current.colors[COLREG_BORDER];
    info.bgColor0 = reg.current.colors[COLREG_BG0];
    info.bgColor1 = reg.current.colors[COLREG_BG1];
    info.bgColor2 = reg.current.colors[COLREG_BG2];
    info.bgColor3 = reg.current.colors[COLREG_BG3];
         
    info.memSelect = memSelect;
    info.ultimax = ultimax;
    info.memoryBankAddr = bankAddr;
    info.screenMemoryAddr = VM13VM12VM11VM10() << 6;
    info.charMemoryAddr = (CB13CB12CB11() << 10) % 0x4000;

    info.irqRasterline = rasterInterruptLine();
    info.imr = imr;
    info.irr = irr;
    
    info.latchedLightPenX = latchedLightPenX;
    info.latchedLightPenY = latchedLightPenY;
    info.lpLine = lpLine;
    info.lpIrqHasOccurred = lpIrqHasOccurred;
    
    return info;
}

SpriteInfo
VICII::getSpriteInfo(unsigned i)
{
    SpriteInfo info;
    
    info.enabled = GET_BIT(reg.current.sprEnable, i);
    info.x = reg.current.sprX[i];
    info.y = reg.current.sprY[i];
    info.color = reg.current.colors[COLREG_SPR0 + i];
    info.extraColor1 = reg.current.colors[COLREG_SPR_EX1];
    info.extraColor2 = reg.current.colors[COLREG_SPR_EX2];
    info.multicolor = GET_BIT(reg.current.sprMC, i);
    info.expandX = GET_BIT(reg.current.sprExpandX, i);
    info.expandY = GET_BIT(reg.current.sprExpandY, i);
    info.priority = GET_BIT(reg.current.sprPriority, i);
    info.collidesWithSprite = GET_BIT(spriteSpriteCollision, i);
    info.collidesWithBackground = GET_BIT(spriteBackgroundColllision, i);
    
    return info;
}

void
VICII::setMemoryBankAddr(u16 addr)
{
    assert(addr % 0x4000 == 0);
    
    suspend();
    bankAddr = addr;
    resume();
}

void
VICII::setScreenMemoryAddr(u16 addr)
{
    assert((addr & ~0x3C00) == 0);
    
    suspend();
    addr >>= 6;
    memSelect = (memSelect & ~0xF0) | (addr & 0xF0);
    resume();
}

void
VICII::setCharacterMemoryAddr(u16 addr)
{
    assert((addr & ~0x3800) == 0);
    
    suspend();
    addr >>= 10;
    memSelect = (memSelect & ~0x0E) | (addr & 0x0E);
    resume();
}

void
VICII::setDisplayMode(DisplayMode m)
{
    suspend();
    reg.current.ctrl1 = (reg.current.ctrl1 & ~0x60) | (m & 0x60);
    reg.current.ctrl2 = (reg.current.ctrl2 & ~0x10) | (m & 0x10);
    delay |= VICUpdateRegisters;
    resume();
}

void
VICII::setNumberOfRows(unsigned rs)
{
    assert(rs == 24 || rs == 25);
    
    suspend();
    u8 cntrl = reg.current.ctrl1;
    WRITE_BIT(cntrl, 3, rs == 25);
    poke(0x11, cntrl);
    resume();
}

void
VICII::setNumberOfColumns(unsigned cs)
{
    assert(cs == 38 || cs == 40);

    suspend();
    u8 cntrl = reg.current.ctrl2;
    WRITE_BIT(cntrl, 3, cs == 40);
    poke(0x16, cntrl);
    resume();
}

ScreenGeometry
VICII::getScreenGeometry(void)
{
    unsigned rows = GET_BIT(reg.current.ctrl1, 3) ? 25 : 24;
    unsigned cols = GET_BIT(reg.current.ctrl2, 3) ? 40 : 38;
    
    if (cols == 40) {
        return rows == 25 ? COL_40_ROW_25 : COL_40_ROW_24;
    } else {
        return rows == 25 ? COL_38_ROW_25 : COL_38_ROW_24;
    }
}

void
VICII::setScreenGeometry(ScreenGeometry mode)
{
    suspend();
    setNumberOfRows((mode == COL_40_ROW_25 || mode == COL_38_ROW_25) ? 25 : 24);
    setNumberOfColumns((mode == COL_40_ROW_25 || mode == COL_40_ROW_24) ? 40 : 38);
    resume();
}

void
VICII::setVerticalRasterScroll(u8 offset)
{
    assert(offset < 8);
    
    suspend();
    reg.current.ctrl1 = (reg.current.ctrl1 & 0xF8) | (offset & 0x07);
    delay |= VICUpdateRegisters;
    resume();
}

void
VICII::setHorizontalRasterScroll(u8 offset)
{
    assert(offset < 8);
    
    suspend();
    reg.current.ctrl2 = (reg.current.ctrl2 & 0xF8) | (offset & 0x07);
    delay |= VICUpdateRegisters;
    resume();
}

void
VICII::setRasterInterruptLine(u16 line)
{
    suspend();
    rasterIrqLine = line & 0xFF;
    WRITE_BIT(reg.delayed.ctrl1, 7, line > 0xFF);
    WRITE_BIT(reg.current.ctrl1, 7, line > 0xFF);
    resume();
}

void
VICII::setRasterInterruptEnable(bool b)
{
    suspend();
    WRITE_BIT(imr, 1, b);
    resume();
}

void
VICII::toggleRasterInterruptFlag()
{
    suspend();
    TOGGLE_BIT(imr, 1);
    resume();
}


//
// Sprites
//

void
VICII::setSpriteX(unsigned nr, u16 x)
{
    assert(nr < 8);
    x = MIN(x, 511);
    
    suspend();
    reg.current.sprX[nr] = x;
    delay |= VICUpdateRegisters;
    resume();
}

void
VICII::setSpriteY(unsigned nr, u8 y)
{
    assert(nr < 8);
    
    suspend();
    reg.current.sprY[nr] = y;
    delay |= VICUpdateRegisters;
    resume();
}

void
VICII::setSpritePtr(unsigned nr, u8 ptr)
{
    assert(nr < 8);
    
    debug(VIC_DEBUG, "setSpritePtr(%d, %d)\n", nr, ptr);
    
    suspend();
    u16 addr = (VM13VM12VM11VM10() << 6) | 0x03F8 | nr;
    mem.ram[addr] = ptr;
    resume();
}

void
VICII::setSpriteColor(unsigned nr, u8 color)
{
    assert(nr < 8);
    
    suspend();
    reg.current.colors[COLREG_SPR0 + nr] = color;
    delay |= VICUpdateRegisters;
    resume();
}

void
VICII::setSpriteEnabled(u8 nr, bool b)
{
    suspend();
    WRITE_BIT(reg.current.sprEnable, nr, b);
    resume();
}

void
VICII::toggleSpriteEnabled(u8 nr)
{
    suspend();
    TOGGLE_BIT(reg.current.sprEnable, nr);
    delay |= VICUpdateRegisters;
    resume();
}

void
VICII::setIrqOnSpriteBackgroundCollision(bool b)
{
    suspend();
    WRITE_BIT(imr, 1, b);
    resume();
}

void
VICII::toggleIrqOnSpriteBackgroundCollision()
{
    suspend();
    TOGGLE_BIT(imr, 1);
    resume();
}

void
VICII::setIrqOnSpriteSpriteCollision(bool b)
{
    suspend();
    WRITE_BIT(imr, 2, b);
    resume();
}

void
VICII::toggleIrqOnSpriteSpriteCollision()
{
    suspend();
    TOGGLE_BIT(imr, 2);
    resume();
}

void
VICII::setSpritePriority(unsigned nr, bool b)
{
    assert(nr < 8);
    
    suspend();
    WRITE_BIT(reg.current.sprPriority, nr, b);
    delay |= VICUpdateRegisters;
    resume();
}

void
VICII::toggleSpritePriority(unsigned nr)
{
    assert(nr < 8);
    
    suspend();
    TOGGLE_BIT(reg.current.sprPriority, nr);
    delay |= VICUpdateRegisters;
    resume();
}

void
VICII::setSpriteMulticolor(unsigned nr, bool b)
{
    assert(nr < 8);
    
    suspend();
    WRITE_BIT(reg.current.sprMC, nr, b);
    delay |= VICUpdateRegisters;
    resume();
}

void
VICII::toggleMulticolorFlag(unsigned nr)
{
    assert(nr < 8);
    
    suspend();
    TOGGLE_BIT(reg.current.sprMC, nr);
    delay |= VICUpdateRegisters;
    resume();
}

void
VICII::setSpriteStretchY(unsigned nr, bool b)
{
    assert(nr < 8);
    
    suspend();
    WRITE_BIT(reg.current.sprExpandY, nr, b);
    delay |= VICUpdateRegisters;
    resume();
}

void
VICII::spriteToggleStretchYFlag(unsigned nr)
{
    assert(nr < 8);
    
    suspend();
    TOGGLE_BIT(reg.current.sprExpandY, nr);
    delay |= VICUpdateRegisters;
    resume();
}

void
VICII::setSpriteStretchX(unsigned nr, bool b)
{
    assert(nr < 8);
    
    suspend();
    WRITE_BIT(reg.current.sprExpandX, nr, b);
    delay |= VICUpdateRegisters;
    resume();
}

void
VICII::spriteToggleStretchXFlag(unsigned nr)
{
    assert(nr < 8);

    suspend();
    TOGGLE_BIT(reg.current.sprExpandX, nr);
    delay |= VICUpdateRegisters;
    resume();
}

void
VICII::setShowIrqLines(bool show)
{
    suspend();
    markIRQLines = show;
    resume();
}

void
VICII::setShowDmaLines(bool show)
{
    suspend();
    markDMALines = show;
    resume();
}

void
VICII::setHideSprites(bool hide)
{
    suspend();
    hideSprites = hide;
    resume();
}

void
VICII::setSpriteSpriteCollisionFlag(bool b)
{
    suspend();
    spriteSpriteCollisionEnabled = b;
    resume();
}

void
VICII::toggleSpriteSpriteCollisionFlag()
{
    suspend();
    spriteSpriteCollisionEnabled = !spriteSpriteCollisionEnabled;
    resume();
}

void
VICII::setSpriteBackgroundCollisionFlag(bool b)
{
    suspend();
    spriteBackgroundCollisionEnabled = b;
    resume();
}

void
VICII::toggleSpriteBackgroundCollisionFlag()
{
    suspend();
    spriteBackgroundCollisionEnabled = !spriteBackgroundCollisionEnabled;
    resume();
}



